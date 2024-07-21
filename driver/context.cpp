#include <sched.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <chrono>
#include <memory>
#include <vector>

#include "commands_gen9.h"
#include "context.h"
#include "gpgpu.h"
#include "ioctl.h"
#include "utils.h"


Context::Context(Device* device) 
        : device(device),
          hwInfo(device->getDeviceDescriptor()->pHwInfo),
          workItemsPerWorkGroup{1, 1, 1},
          globalWorkItems{1, 1, 1},
          numWorkGroups{1, 1, 1},             //TODO: Check if all ones is correct here
          isMidThreadLevelPreemptionSupported(device->getMidThreadPreemptionSupport()) {
    setMaxWorkGroupSize();
    setMaxThreadsForVfe();
}

Context::~Context() {
    DBG_LOG("[DEBUG] Context destructor called!\n");
    drm_i915_gem_context_destroy destroy = {0};
    destroy.ctx_id = this->ctxId;
    int ret = ioctl(device->fd, DRM_IOCTL_I915_GEM_CONTEXT_DESTROY, &destroy);
    if (ret)
        DBG_LOG("[DEBUG] ioctl(I915_GEM_CONTEXT_DESTROY) failed with error %d\n", ret);

    drm_i915_gem_vm_control vmCtrl = {0};
    vmCtrl.vm_id = this->vmId;
    ret = ioctl(device->fd, DRM_IOCTL_I915_GEM_VM_DESTROY, &vmCtrl);
    if (ret)
        DBG_LOG("[DEBUG] ioctl(I915_GEM_VM_DESTROY) failed with error %d\n", ret);
}

BufferObject::BufferObject(int fd, int bufferType, void* cpuAddress, uint32_t handle, size_t size)
        : fd(fd),
          bufferType(bufferType),
          cpuAddress(cpuAddress),
          handle(handle),
          size(size) {
}

BufferObject::~BufferObject() {
    DBG_LOG("[DEBUG] BufferObject destructor called for type [%d]\n", this->bufferType);
    alignedFree(this->cpuAddress);
    deleteHandle();
}

void BufferObject::deleteHandle() {
    // Make sure the BO isn't used anymore.
    drm_i915_gem_wait wait = {0};
    wait.bo_handle = this->handle;
    wait.timeout_ns = -1;
    int ret = ioctl(this->fd, DRM_IOCTL_I915_GEM_WAIT, &wait);
    if (ret)
        DBG_LOG("[DEBUG] ioctl(I915_GEM_WAIT) failed with error %d\n", ret);
    // Delete the BO handle.
    drm_gem_close close = {0};
    close.handle = this->handle;
    ret = ioctl(this->fd, DRM_IOCTL_GEM_CLOSE, &close);
    if (ret)
        DBG_LOG("[DEBUG] ioctl(GEM_CLOSE) failed with error %d\n", ret);
}


void Context::setMaxWorkGroupSize() {
    uint32_t numThreadsPerEU = hwInfo->gtSystemInfo->ThreadCount / hwInfo->gtSystemInfo->EUCount;
    uint32_t maxThreadsPerWorkGroup = hwInfo->gtSystemInfo->MaxEuPerSubSlice * numThreadsPerEU;
    maxThreadsPerWorkGroup = prevPowerOfTwo(maxThreadsPerWorkGroup);
    this->maxWorkItemsPerWorkGroup = std::min(maxThreadsPerWorkGroup, 1024u);
}

void Context::setMaxThreadsForVfe() {
    // For GEN11 and GEN12, there is another term (extraQuantityThreadsPerEU) that must be added to numThreadsPerEU
    uint32_t numThreadsPerEU = hwInfo->gtSystemInfo->ThreadCount / hwInfo->gtSystemInfo->EUCount;
    maxVfeThreads = hwInfo->gtSystemInfo->EUCount * numThreadsPerEU;
}

void Context::setKernelData(KernelFromPatchtokens* kernelData) {
    this->kernelData = kernelData;
}

KernelFromPatchtokens* Context::getKernelData() {
    return kernelData;
}

BufferObject* Context::getBatchBuffer() {
    return dataBatchBuffer.get();
}

bool Context::isSIPKernelAllocated() {
    return isSipKernelAllocated;
}

bool Context::isGraphicsBaseAddressRequired(int bufferType) {
    return bufferType == BufferType::INTERNAL_HEAP ||
           bufferType == BufferType::KERNEL_ISA ||
           bufferType == BufferType::KERNEL_ISA_INTERNAL;
}


std::unique_ptr<BufferObject> Context::allocateBufferObject(size_t size, int bufferType) {
    size_t alignment = MemoryConstants::pageSize;
    size_t sizeToAlloc = size + alignment;
    char* pOriginalMemory = new (std::nothrow)char[sizeToAlloc];
    if (!pOriginalMemory)
        return nullptr;
    // The allocated memory must be aligned to a page boundary in order to be correctly 
    // bound into the ppGTT.
    uintptr_t pAlignedMemory = reinterpret_cast<uintptr_t>(pOriginalMemory);
    pAlignedMemory += alignment;
    pAlignedMemory -= pAlignedMemory % alignment;
    reinterpret_cast<void**>(pAlignedMemory)[-1] = pOriginalMemory;
    void* pAlignedMemoryPtr = reinterpret_cast<void*>(pAlignedMemory);

    // This ioctl allows the kernel driver to keep track of all buffer objects (BOs). 
    // A handle for each BO is created, but the BOs are not yet mapped into the ppGTT
    // (this is done by DRM_IOCTL_I915_GEM_EXECBUFFER2 ioctl).
    drm_i915_gem_userptr userptr = {0};
    userptr.user_ptr = pAlignedMemory;
    userptr.user_size = size;
    userptr.flags = 0;
    int ret = ioctl(device->fd, DRM_IOCTL_I915_GEM_USERPTR, &userptr);
    if (ret) {
        alignedFree(pAlignedMemoryPtr);
        return nullptr;
    }
    auto bo = std::make_unique<BufferObject>(device->fd, bufferType, pAlignedMemoryPtr, userptr.handle, size);
    if (isGraphicsBaseAddressRequired(bufferType)) {
        ret = device->allocateHeapMemoryForSoftpinning(bo.get());
        if (ret) {
            alignedFree(pAlignedMemoryPtr);
            bo->deleteHandle();
            return nullptr;
        }
    } else {
        bo->gpuAddress = reinterpret_cast<uint64_t>(pAlignedMemory);
    }
    return bo;
}



int Context::createDRMContext() {
    // Create a per-process Graphics Translation Table (ppGTT). This is a process-assigned
    // page table that the IOMMU uses to translate virtual GPU addresses.
    drm_i915_gem_vm_control vmCtrl = {0};
    int ret = ioctl(device->fd, DRM_IOCTL_I915_GEM_VM_CREATE, &vmCtrl);
    vmId = vmCtrl.vm_id;
    if (ret || vmCtrl.vm_id == 0)
        return CONTEXT_CREATION_FAILED;
    
    // Create a DRM context.
    drm_i915_gem_context_create_ext drmCtx = {0};
    ret = ioctl(device->fd, DRM_IOCTL_I915_GEM_CONTEXT_CREATE_EXT, &drmCtx);
    ctxId = drmCtx.ctx_id;
    if (ret)
        return CONTEXT_CREATION_FAILED;

    // Assign the ppGTT to the newly created context.
    drm_i915_gem_context_param paramVm = {0};
    paramVm.ctx_id = drmCtx.ctx_id;
    paramVm.value = vmCtrl.vm_id;
    paramVm.param = I915_CONTEXT_PARAM_VM;
    ret = ioctl(device->fd, DRM_IOCTL_I915_GEM_CONTEXT_SETPARAM, &paramVm);
    if (ret)
        return CONTEXT_CREATION_FAILED;

    // Check if non-persistent contexts are supported. A non-persistent context gets
    // destroyed immediately upon closure (through DRM_I915_GEM_CONTEXT_CLOSE, device file
    // closure or process termination). A persistent context can finish the batch despite
    // closure of the underlying CPU process.
    drm_i915_gem_context_param paramPers = {0};
    paramPers.param = I915_CONTEXT_PARAM_PERSISTENCE;
    ret = ioctl(device->fd, DRM_IOCTL_I915_GEM_CONTEXT_GETPARAM, &paramPers);
    if (ret == 0 && paramPers.value == 1) {
        // Makes the context non-persistent
        drm_i915_gem_context_param paramSetPers = {0};
        paramSetPers.ctx_id = drmCtx.ctx_id;
        paramSetPers.param = I915_CONTEXT_PARAM_PERSISTENCE;
        ioctl(device->fd, DRM_IOCTL_I915_GEM_CONTEXT_SETPARAM, &paramSetPers);
    }

    // Even though my test machine (Skylake) has Turbo Boost 2.0, this does not work. 
    // TODO: Use ftrace to see why we get EINVAL error
    drm_i915_gem_context_param paramBoost = {0};
    paramBoost.ctx_id = drmCtx.ctx_id;
    paramBoost.param = I915_CONTEXT_PRIVATE_PARAM_BOOST;
    paramBoost.value = 1;
    ret = ioctl(device->fd, DRM_IOCTL_I915_GEM_CONTEXT_SETPARAM, &paramBoost);

    return SUCCESS;
}


int Context::allocateReusableBufferObjects() {
    // Before a GPU context switch, the current state of the GPU (registers, instruction
    // pointer, etc.) is being copied into the preemption buffer.
    size_t preemptionSize = hwInfo->gtSystemInfo->CsrSizeInMb * MemoryConstants::megaByte;
    preemptionAllocation = allocateBufferObject(preemptionSize, BufferType::PREEMPTION);
    if (!preemptionAllocation)
        return BUFFER_ALLOCATION_FAILED;

    // When kernel execution has finished, the hardware (or kernel driver, I'm not sure)
    // notifies this to userspace by placing a tag value into this tag buffer.
    tagAllocation = allocateBufferObject(MemoryConstants::pageSize, BufferType::TAG_BUFFER);
    if (!tagAllocation)
        return BUFFER_ALLOCATION_FAILED;
    uint32_t* tagAddress = reinterpret_cast<uint32_t*>(tagAllocation->cpuAddress);
    uint32_t initialHardwareTag = 0u;
    *tagAddress = initialHardwareTag;
    uint8_t* tagAdd = reinterpret_cast<uint8_t*>(tagAllocation->cpuAddress);
    DebugPauseState* debugPauseStateAddress = reinterpret_cast<DebugPauseState*>(tagAdd + MemoryConstants::cacheLineSize);
    *debugPauseStateAddress = DebugPauseState::waitingForFirstSemaphore;

    // CreateBuffer API pins data buffer objects directly into GPU address space. This
    // will be the batch buffer for this operation.
    dataBatchBuffer = allocateBufferObject(MemoryConstants::pageSize, BufferType::COMMAND_BUFFER);
    if (!dataBatchBuffer)
        return BUFFER_ALLOCATION_FAILED;
    // Program MI_BATCH_BUFFER_END
    auto cmd1 = dataBatchBuffer->ptrOffset<MI_BATCH_BUFFER_END*>(sizeof(MI_BATCH_BUFFER_END));
    *cmd1 = MI_BATCH_BUFFER_END::init();
    // Program NOOP
    auto cmd2 = dataBatchBuffer->ptrOffset<uint32_t*>(sizeof(uint32_t));
    *cmd2 = 0u;

    return SUCCESS;
}


/*
- Total number of work items must be specified, otherwise it alway returns INVALID_WORK_GROUP_SIZE
- If local_work_size is nullptr, then workItemsPerWorkGroup[i] will always be 1, which leads to INVALID_WORK_GROUP_SIZE
*/
int Context::validateWorkGroups(uint32_t work_dim, const size_t* global_work_size, const size_t* local_work_size) {
    if (work_dim > 3) {
        return INVALID_WORK_GROUP_SIZE;
    }
    size_t requiredWorkGroupSize[3] = {kernelData->executionEnvironment->RequiredWorkGroupSizeX,
                                       kernelData->executionEnvironment->RequiredWorkGroupSizeY,
                                       kernelData->executionEnvironment->RequiredWorkGroupSizeZ};
    size_t totalWorkItems = 1u;
    size_t remainder = 0;
    bool haveRequiredWorkGroupSize = false;
    if (requiredWorkGroupSize[0] != 0) {
        haveRequiredWorkGroupSize = true;
    }
    for (uint32_t i = 0u; i < work_dim; i++) {
        globalWorkItems[i] = global_work_size ? global_work_size[i] : 0;
        if (local_work_size) {
            if (haveRequiredWorkGroupSize) {
                if (requiredWorkGroupSize[i] != local_work_size[i]) {
                    return INVALID_WORK_GROUP_SIZE;
                }
            }
            if (local_work_size[i] == 0) {
                return INVALID_WORK_GROUP_SIZE;
            }
            workItemsPerWorkGroup[i] = local_work_size[i];
            totalWorkItems *= local_work_size[i];
        }
        remainder += globalWorkItems[i] % workItemsPerWorkGroup[i];
    }
    if (remainder != 0) {
        return INVALID_WORK_GROUP_SIZE;
    }
    if (totalWorkItems > maxWorkItemsPerWorkGroup) {
        return INVALID_WORK_GROUP_SIZE;
    }
    for (uint32_t i = 0u; i < work_dim; i++) {
        numWorkGroups[i] = globalWorkItems[i] / workItemsPerWorkGroup[i];
    }
    return SUCCESS;
}


int Context::constructBufferObjects() {
    int ret = allocateISAMemory();
    if (ret)
        return ret;
    ret = createScratchAllocation();
    if (ret)
        return ret;
    ret = createSurfaceStateHeap();
    if (ret)
        return ret;
    ret = createIndirectObjectHeap();
    if (ret)
        return ret;
    ret = createDynamicStateHeap();
    if (ret)
        return ret;
    ret = createCommandStreamTask();
    if (ret)
        return ret;
    ret = createCommandStreamReceiver();
    if (ret)
        return ret;
    return SUCCESS;
}


int Context::allocateISAMemory() {
    size_t kernelISASize = static_cast<size_t>(kernelData->header->KernelHeapSize);
    size_t alignedAllocationSize = alignUp(kernelISASize, MemoryConstants::pageSize);
    bool hasRequiredAllocationSize;
    if (kernelAllocation) {
        hasRequiredAllocationSize = (kernelAllocation->size >= alignedAllocationSize) ? true : false;
    }
    if (!kernelAllocation || !hasRequiredAllocationSize) {
        kernelAllocation.reset(); // after freeing, the kernel driver still holds a handle to this object, but I don't think this causes issues
        kernelAllocation = allocateBufferObject(alignedAllocationSize, BufferType::KERNEL_ISA);
        if (!kernelAllocation)
            return KERNEL_ALLOCATION_FAILED;
    }
    memcpy(kernelAllocation->cpuAddress, kernelData->isa, kernelISASize);
    return SUCCESS;
}


//TODO: Check why Intels GEMM uses no Scratch Space
//TODO: Check necessary alignment size
//TODO: Make sure that mediaVfeState[0] is never nullptr
int Context::createScratchAllocation() {
    uint32_t computeUnitsUsedForScratch = hwInfo->gtSystemInfo->MaxSubSlicesSupported
                                        * hwInfo->gtSystemInfo->MaxEuPerSubSlice
                                        * hwInfo->gtSystemInfo->ThreadCount
                                        / hwInfo->gtSystemInfo->EUCount;
    if (!scratchAllocation) {
        uint32_t requiredScratchSize = kernelData->mediaVfeState[0]->PerThreadScratchSpace;
        size_t requiredScratchSizeInBytes = requiredScratchSize * computeUnitsUsedForScratch;
        if (requiredScratchSize) {
            size_t alignedAllocationSize = alignUp(requiredScratchSizeInBytes, MemoryConstants::pageSize);
            scratchAllocation = allocateBufferObject(alignedAllocationSize, BufferType::SCRATCH_SURFACE);
            if (!scratchAllocation)
                return BUFFER_ALLOCATION_FAILED;
            requiredScratchSize >>= static_cast<uint32_t>(MemoryConstants::kiloByteShiftSize);
            while (requiredScratchSize >>= 1) {
                perThreadScratchSpace++;
            }
            return SUCCESS;
        }
        perThreadScratchSpace = 0u;
    }
    return SUCCESS;
}


int Context::createSurfaceStateHeap() {
    size_t sshSize = static_cast<size_t>(kernelData->header->SurfaceStateHeapSize);
    if (!sshAllocation) {
        sshAllocation = allocateBufferObject(16 * MemoryConstants::pageSize, BufferType::LINEAR_STREAM);
        if (!sshAllocation)
            return BUFFER_ALLOCATION_FAILED;
        sshAllocation->offset += sizeof(RENDER_SURFACE_STATE);
    }
    uint32_t numberOfBindingTableStates = kernelData->bindingTableState->Count;
    if (numberOfBindingTableStates == 0)
        return 0;
    char* srcSsh = kernel->getSurfaceStatePtr();
    uint32_t surfaceStatesOffset = sshAllocation->offset;
    void* dstSurfaceState = ptrOffset(sshAllocation->cpuAddress, surfaceStatesOffset);
    memcpy(dstSurfaceState, srcSsh, sshSize);

    uint32_t offsetOfBindingTable = kernelData->bindingTableState->Offset;
    auto srcBtiTableBase = reinterpret_cast<BINDING_TABLE_STATE*>(ptrOffset(srcSsh, offsetOfBindingTable));
    auto dstBtiTableBase = reinterpret_cast<BINDING_TABLE_STATE*>(ptrOffset(dstSurfaceState, offsetOfBindingTable));
    BINDING_TABLE_STATE bti = BINDING_TABLE_STATE::init();
    for (uint32_t i = 0, e = numberOfBindingTableStates; i != e; ++i) {
        uint32_t localSurfaceStateOffset = srcBtiTableBase[i].Bitfield.SurfaceStatePointer << 0x6;
        uint32_t offsetedSurfaceStateOffset = localSurfaceStateOffset + surfaceStatesOffset;
        bti.Bitfield.SurfaceStatePointer = offsetedSurfaceStateOffset >> 0x6;
        dstBtiTableBase[i] = bti;
    }
    //provisorisch:
    sshAllocation->offset += numberOfBindingTableStates * sizeof(RENDER_SURFACE_STATE);
    sshAllocation->offset += numberOfBindingTableStates * sizeof(BINDING_TABLE_STATE);
    sshAllocation->offset = alignUp(sshAllocation->offset, MemoryConstants::cacheLineSize);

    return SUCCESS;
}


int Context::createIndirectObjectHeap() {
    if (!iohAllocation) {
        iohAllocation = allocateBufferObject(16 * MemoryConstants::pageSize, BufferType::INTERNAL_HEAP);
        if (!iohAllocation)
            return BUFFER_ALLOCATION_FAILED;
    }
    iohAllocation->gpuAddress = canonize(0x8001fffd6000);
    // Patch CrossThreadData
    char* crossThreadData = kernel->getCrossThreadData();
    for (uint32_t i = 0; i < 3; i++) {
        patchKernelConstant(kernelData->crossThreadPayload.localWorkSize[i], crossThreadData, workItemsPerWorkGroup[i]);
        patchKernelConstant(kernelData->crossThreadPayload.globalWorkSize[i], crossThreadData, globalWorkItems[i]);
        patchKernelConstant(kernelData->crossThreadPayload.numWorkGroups[i], crossThreadData, numWorkGroups[i]);
    }
    patchKernelConstant(kernelData->crossThreadPayload.workDimensions, crossThreadData, workDim);
    this->crossThreadDataSize = kernelData->dataParameterStream->DataParameterStreamSize;
    void* crossThreadDataAddress = ptrOffset(iohAllocation->cpuAddress, iohAllocation->offset);
    memcpy(crossThreadDataAddress, crossThreadData, crossThreadDataSize);

    // Calculate number of hardware threads needed for one work group
    size_t localWorkSize = workItemsPerWorkGroup[0] * workItemsPerWorkGroup[1] * workItemsPerWorkGroup[2];
    uint32_t simdSize = kernelData->executionEnvironment->LargestCompiledSIMDSize;
    uint64_t threadsPerWG = simdSize + localWorkSize - 1;
    threadsPerWG >>= simdSize == 32 ? 5 : simdSize == 16 ? 4 : simdSize == 8 ? 3 : 0;
    this->hwThreadsPerWorkGroup = threadsPerWG;

    iohAllocation->offset += static_cast<size_t>(crossThreadDataSize);
    //generateLocalIDsSimd(iohAllocation->cpuAddress, threadsPerWG, simdSize);
    generateLocalIDs(iohAllocation.get());

    // Calculate total size of PerThreadData
    this->GRFSize = 32; // one general purpose register file (GRF) has 32 bytes on GEN9
    uint32_t numLocalIdChannels = kernelData->threadPayload->LocalIDXPresent
                                + kernelData->threadPayload->LocalIDYPresent
                                + kernelData->threadPayload->LocalIDZPresent;
    uint32_t numGRFsPerThread = (simdSize == 32 && GRFSize == 32) ? 2 : 1;
    uint32_t localIDSizePerThread = numGRFsPerThread * GRFSize * (simdSize == 1 ? 1u : numLocalIdChannels);
    localIDSizePerThread = std::max(localIDSizePerThread, GRFSize);
    this->perThreadDataSize = this->hwThreadsPerWorkGroup * localIDSizePerThread;

    // Align to cache line
    iohAllocation->offset += perThreadDataSize;
    iohAllocation->offset = alignUp(iohAllocation->offset, MemoryConstants::cacheLineSize);

    return SUCCESS;
}

void Context::patchKernelConstant(const PatchDataParameterBuffer* info, char* crossThreadData, size_t kernelConstant) {
    if (info) {
        uint32_t patchOffset = info->Offset;
        uint32_t* patchPtr = reinterpret_cast<uint32_t*>(ptrOffset(crossThreadData, patchOffset));
        *patchPtr = static_cast<uint32_t>(kernelConstant);
    }
}

void Context::generateLocalIDs(BufferObject* ioh) {
    uint16_t* iohOffset = static_cast<uint16_t*>(ptrOffset(ioh->cpuAddress, ioh->offset));
    memset(iohOffset, 0x0, 96);
    for (int i = 0; i < 16; i++) {
        iohOffset[i] = i;
    }
}

alignas(32)
const uint16_t initialLocalID[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};

void Context::generateLocalIDsSimd(void* ioh, uint16_t threadsPerWorkGroup, uint32_t simdSize) {
    const uint32_t numChannels = 8u;
    const int passes = simdSize / numChannels;
    int pass = 0;
    uint32_t dimNum[3] = {0, 1, 2};

    __m256i vLwsX = _mm256_set1_epi16(workItemsPerWorkGroup[dimNum[0]]);
    __m256i vLwsY = _mm256_set1_epi16(workItemsPerWorkGroup[dimNum[1]]);

    __m256i zero = _mm256_set1_epi16(0u);
    __m256i one = _mm256_set1_epi16(1u);

    const uint64_t threadSkipSize = ((simdSize == 32) ? 32 : 16) * sizeof(uint16_t);

    __m256i vSimdX = _mm256_set1_epi16(simdSize);
    __m256i vSimdY = zero;
    __m256i vSimdZ = zero;

    __m256i xWrap;
    __m256i yWrap;

    bool isZero1, isZero2;

    do {
        xWrap = vSimdX >= vLwsX;
        __m256i deltaX = _mm256_blendv_epi8(vLwsX, zero, xWrap);
        vSimdX -= deltaX;
        __m256i deltaY = _mm256_blendv_epi8(one, zero, xWrap);
        vSimdY += deltaY;
        yWrap = vSimdY >= vLwsY;
        __m256i deltaY2 = _mm256_blendv_epi8(vLwsY, zero, yWrap);
        vSimdY -= deltaY2;
        __m256i deltaZ = _mm256_blendv_epi8(one, zero, yWrap);
        vSimdZ += deltaZ;
        isZero1 = __builtin_ia32_ptestz256((__v4di)xWrap, (__v4di)xWrap);
        isZero2 = __builtin_ia32_ptestz256((__v4di)yWrap, (__v4di)yWrap);
    } while (!isZero1 || !isZero2);
    
    do {
        void* buffer = ioh;

        __m256i x = _mm256_load_si256(&initialLocalID[pass * numChannels]);
        __m256i y = zero;
        __m256i z = zero;

        do {
            xWrap = x >= vLwsX;
            __m256i deltaX = _mm256_blendv_epi8(vLwsX, zero, xWrap);
            x -= deltaX;
            __m256i deltaY = _mm256_blendv_epi8(one, zero, xWrap);
            y += deltaY;
            yWrap = y >= vLwsY;
            __m256i deltaY2 = _mm256_blendv_epi8(vLwsY, zero, yWrap);
            y -= deltaY2;
            __m256i deltaZ = _mm256_blendv_epi8(one, zero, yWrap);
            z += deltaZ;
            isZero1 = __builtin_ia32_ptestz256((__v4di)xWrap, (__v4di)xWrap);
        } while (!isZero1);

        for (size_t i = 0; i < threadsPerWorkGroup; ++i) {
            __mm256_store_si256(reinterpret_cast<__m256i*>(ptrOffset(buffer, dimNum[0] * threadSkipSize)), x);
            __mm256_store_si256(reinterpret_cast<__m256i*>(ptrOffset(buffer, dimNum[1] * threadSkipSize)), y);
            __mm256_store_si256(reinterpret_cast<__m256i*>(ptrOffset(buffer, dimNum[2] * threadSkipSize)), z);

            x += vSimdX;
            y += vSimdY;
            z += vSimdZ;
            xWrap = x >= vLwsX;
            __m256i deltaX = _mm256_blendv_epi8(vLwsX, zero, xWrap);
            x -= deltaX;
            __m256i deltaY = _mm256_blendv_epi8(one, zero, xWrap);
            y += deltaY;
            yWrap = y >= vLwsY;
            __m256i deltaY2 = _mm256_blendv_epi8(one, zero, yWrap);
            y -= deltaY2;
            __m256i deltaZ = _mm256_blendv_epi8(one, zero, yWrap);
            z += deltaZ;
            buffer = ptrOffset(buffer, 3 * threadSkipSize);
        }
        ioh = ptrOffset(ioh, 8 * sizeof(uint16_t));
    } while (++pass < passes);
}



int Context::createDynamicStateHeap() {
    if (!dshAllocation) {
        dshAllocation = allocateBufferObject(16 * MemoryConstants::pageSize, BufferType::LINEAR_STREAM);
        if (!dshAllocation)
            return BUFFER_ALLOCATION_FAILED;
    }
    auto interfaceDescriptor = dshAllocation->ptrOffset<INTERFACE_DESCRIPTOR_DATA*>(sizeof(INTERFACE_DESCRIPTOR_DATA));
    *interfaceDescriptor = INTERFACE_DESCRIPTOR_DATA::init();
    uint64_t kernelStartOffset = kernelAllocation->gpuAddress - kernelAllocation->gpuBaseAddress;
    interfaceDescriptor->Bitfield.KernelStartPointerHigh = kernelStartOffset >> 32;
    interfaceDescriptor->Bitfield.KernelStartPointer = static_cast<uint32_t>(kernelStartOffset) >> 0x6;
    interfaceDescriptor->Bitfield.DenormMode = INTERFACE_DESCRIPTOR_DATA::DENORM_MODE_SETBYKERNEL;
    interfaceDescriptor->Bitfield.BindingTableEntryCount = std::min(kernelData->bindingTableState->Count, 31u);
    //uint32_t bindingTableOffset = kernelData->bindingTableState->Offset + sizeof(RENDER_SURFACE_STATE);
    // provisorisch:
    uint32_t bindingTableOffset = sshAllocation->offset - MemoryConstants::cacheLineSize;
    interfaceDescriptor->Bitfield.BindingTablePointer = bindingTableOffset >> 0x5;
    interfaceDescriptor->Bitfield.SharedLocalMemorySize = 0u;
    interfaceDescriptor->Bitfield.NumberOfThreadsInGpgpuThreadGroup = static_cast<uint32_t>(this->hwThreadsPerWorkGroup);
    interfaceDescriptor->Bitfield.CrossThreadConstantDataReadLength = this->crossThreadDataSize / this->GRFSize;
    uint32_t numGrfPerThreadData = static_cast<uint32_t>(this->perThreadDataSize / this->GRFSize);
    numGrfPerThreadData = std::max(numGrfPerThreadData, 1u);
    interfaceDescriptor->Bitfield.ConstantIndirectUrbEntryReadLength = numGrfPerThreadData;
    interfaceDescriptor->Bitfield.BarrierEnable = kernelData->executionEnvironment->HasBarriers;

    dshAllocation->offset = alignUp(dshAllocation->offset, MemoryConstants::cacheLineSize);

    return SUCCESS;
}


int Context::createSipAllocation(size_t sipSize, const char* sipBinaryRaw) {
    size_t sipAllocSize = alignUp(sipSize, MemoryConstants::pageSize);
    sipAllocation = allocateBufferObject(sipAllocSize, BufferType::KERNEL_ISA_INTERNAL);
    if (!sipAllocation)
        return BUFFER_ALLOCATION_FAILED;
    memcpy(sipAllocation->cpuAddress, sipBinaryRaw, sipSize);
    isSipKernelAllocated = true;
    return SUCCESS;
}

int Context::createCommandStreamTask() {
    //TODO: Reset offset value if EnqueueNDRangeKernel is called multiple times
    if (!commandStreamTask) {
        commandStreamTask = allocateBufferObject(16 * MemoryConstants::pageSize, BufferType::COMMAND_BUFFER);
        if (!commandStreamTask)
            return BUFFER_ALLOCATION_FAILED;
    }
    // Program MEDIA_STATE_FLUSH
    auto cmd1 = commandStreamTask->ptrOffset<MEDIA_STATE_FLUSH*>(sizeof(MEDIA_STATE_FLUSH));
    *cmd1 = MEDIA_STATE_FLUSH::init();

    // Program MEDIA_INTERFACE_DESCRIPTOR_LOAD
    auto cmd2 = commandStreamTask->ptrOffset<MEDIA_INTERFACE_DESCRIPTOR_LOAD*>(sizeof(MEDIA_INTERFACE_DESCRIPTOR_LOAD));
    *cmd2 = MEDIA_INTERFACE_DESCRIPTOR_LOAD::init();
    cmd2->Bitfield.InterfaceDescriptorDataStartAddress = dshAllocation->offset - alignUp(sizeof(INTERFACE_DESCRIPTOR_DATA), MemoryConstants::cacheLineSize);
    cmd2->Bitfield.InterfaceDescriptorTotalLength = sizeof(INTERFACE_DESCRIPTOR_DATA);

    // Program GPGPU_WALKER
    auto cmd3 = commandStreamTask->ptrOffset<GPGPU_WALKER*>(sizeof(GPGPU_WALKER));
    *cmd3 = GPGPU_WALKER::init();
    uint32_t indirectDataLength = alignUp(crossThreadDataSize + perThreadDataSize, GPGPU_WALKER::INDIRECTDATASTARTADDRESS_ALIGN_SIZE);
    uint64_t iohAddressOffset = iohAllocation->gpuAddress - iohAllocation->gpuBaseAddress;
    iohAddressOffset += iohAllocation->offset;
    iohAddressOffset -= indirectDataLength;
    cmd3->Bitfield.IndirectDataStartAddress = iohAddressOffset >> 0x6;
    uint32_t interfaceDescriptorIndex = 0u;
    cmd3->Bitfield.InterfaceDescriptorOffset = interfaceDescriptorIndex;
    cmd3->Bitfield.IndirectDataLength = indirectDataLength;
    cmd3->Bitfield.ThreadWidthCounterMaximum = static_cast<uint32_t>(this->hwThreadsPerWorkGroup) - 1;
    cmd3->Bitfield.ThreadGroupIdXDimension = static_cast<uint32_t>(numWorkGroups[0]);
    cmd3->Bitfield.ThreadGroupIdYDimension = static_cast<uint32_t>(numWorkGroups[1]);
    cmd3->Bitfield.ThreadGroupIdZDimension = static_cast<uint32_t>(numWorkGroups[2]);
    size_t localWorkSize = workItemsPerWorkGroup[0] * workItemsPerWorkGroup[1] * workItemsPerWorkGroup[2];
    uint32_t simdSize = kernelData->executionEnvironment->LargestCompiledSIMDSize;
    auto remainderSimdLanes = localWorkSize & (simdSize - 1);
    uint64_t executionMask = maxNBitValue(remainderSimdLanes);
    if (!executionMask)
        executionMask = ~executionMask;
    cmd3->Bitfield.RightExecutionMask = static_cast<uint32_t>(executionMask);
    cmd3->Bitfield.BottomExecutionMask = static_cast<uint32_t>(0xffffffff);
    cmd3->Bitfield.SimdSize = (simdSize == 1) ? (32 >> 4) : (simdSize >> 4);
    cmd3->Bitfield.ThreadGroupIdStartingX = 0u;
    cmd3->Bitfield.ThreadGroupIdStartingY = 0u;
    cmd3->Bitfield.ThreadGroupIdStartingResumeZ = 0u;

    // Program Media State Flush
    auto cmd4 = commandStreamTask->ptrOffset<MEDIA_STATE_FLUSH*>(sizeof(MEDIA_STATE_FLUSH));
    *cmd4 = MEDIA_STATE_FLUSH::init();
    cmd4->Bitfield.InterfaceDescriptorOffset = interfaceDescriptorIndex;

    // Program Pipe Control Workaround
    auto cmd5 = commandStreamTask->ptrOffset<PIPE_CONTROL*>(sizeof(PIPE_CONTROL));
    *cmd5 = PIPE_CONTROL::init();
    cmd5->Bitfield.CommandStreamerStallEnable = true;

    // Program Pipe Control with Post Sync Operation
    auto cmd6 = commandStreamTask->ptrOffset<PIPE_CONTROL*>(sizeof(PIPE_CONTROL));
    *cmd6 = PIPE_CONTROL::init();
    cmd6->Bitfield.CommandStreamerStallEnable = true;
    cmd6->Bitfield.NotifyEnable = true;
    cmd6->Bitfield.DcFlushEnable = true;
    cmd6->Bitfield.PostSyncOperation = PIPE_CONTROL::POST_SYNC_OPERATION_WRITE_IMMEDIATE_DATA;
    uint64_t tagAddress = tagAllocation->gpuAddress;
    cmd6->Bitfield.Address = static_cast<uint32_t>(tagAddress & 0x0000ffffffffull) >> 0x2;
    cmd6->Bitfield.AddressHigh = static_cast<uint32_t>(tagAddress >> 32);
    this->completionTag += 1;
    cmd6->Bitfield.ImmediateData = this->completionTag;

    // Program MI_BATCH_BUFFER_END
    auto cmd7 = commandStreamTask->ptrOffset<MI_BATCH_BUFFER_END*>(sizeof(MI_BATCH_BUFFER_END));
    *cmd7 = MI_BATCH_BUFFER_END::init();

    alignToCacheLine(commandStreamTask.get());

    return SUCCESS;
}


int Context::createCommandStreamReceiver() {
    //TODO: Reset offset value if EnqueueNDRangeKernel is called multiple times
    //TODO: This doesn't seem to work if it runs in a loop more than two times
    if (!commandStreamCSR) {
        commandStreamCSR = allocateBufferObject(16 * MemoryConstants::pageSize, BufferType::COMMAND_BUFFER);
        if (!commandStreamCSR)
            return BUFFER_ALLOCATION_FAILED;
    }
    taskCount += 1;
    if (taskCount == 1) {
        // Program Pipeline Selection
        auto cmd1 = commandStreamCSR->ptrOffset<PIPELINE_SELECT*>(sizeof(PIPELINE_SELECT));
        *cmd1 = PIPELINE_SELECT::init();
        uint32_t enablePipelineSelectMaskBits = 0x3;
        uint32_t mediaSamplerDopClockGateMaskBits = 0x10;
        cmd1->Bitfield.MaskBits = enablePipelineSelectMaskBits | mediaSamplerDopClockGateMaskBits;
        cmd1->Bitfield.PipelineSelection = PIPELINE_SELECT::PIPELINE_SELECTION_GPGPU;
        // Clock Gating is a technique to dynamically adjust the clock frequency of the Media
        // Pipeline Hardware. It contains texture mapping units (TMUs) which are responsible for
        // texture sampling. Since this driver doesn't support sampler objects, it makes sense
        // to enable Clock Gating to save power.
        cmd1->Bitfield.MediaSamplerDopClockGateEnable = true;

        // Program L3 Cache:
        auto cmd2 = commandStreamCSR->ptrOffset<MI_LOAD_REGISTER_IMM*>(sizeof(MI_LOAD_REGISTER_IMM));
        *cmd2 = MI_LOAD_REGISTER_IMM::init();
        uint32_t L3ValueNoSLM = 0x80000340;
        cmd2->Bitfield.RegisterOffset = MMIOAddresses::L3Register >> 0x2;
        cmd2->Bitfield.DataDword = L3ValueNoSLM;

        // Program Thread Arbitration
        auto cmd3 = commandStreamCSR->ptrOffset<PIPE_CONTROL*>(sizeof(PIPE_CONTROL));
        *cmd3 = PIPE_CONTROL::init();
        cmd3->Bitfield.CommandStreamerStallEnable = true;

        auto cmd4 = commandStreamCSR->ptrOffset<MI_LOAD_REGISTER_IMM*>(sizeof(MI_LOAD_REGISTER_IMM));
        *cmd4 = MI_LOAD_REGISTER_IMM::init();
        uint32_t requiredThreadArbitrationPolicy = ThreadArbitrationPolicy::RoundRobin;
        cmd4->Bitfield.RegisterOffset = MMIOAddresses::debugControlReg2 >> 0x2;
        cmd4->Bitfield.DataDword = requiredThreadArbitrationPolicy;

        // Program Preemption
        auto cmd5 = commandStreamCSR->ptrOffset<GPGPU_CSR_BASE_ADDRESS*>(sizeof(GPGPU_CSR_BASE_ADDRESS));
        *cmd5 = GPGPU_CSR_BASE_ADDRESS::init();
        cmd5->Bitfield.GpgpuCsrBaseAddress = preemptionAllocation->gpuAddress >> 0xc;

        // Program Pipe Control
        auto cmd6 = commandStreamCSR->ptrOffset<PIPE_CONTROL*>(sizeof(PIPE_CONTROL));
        *cmd6 = PIPE_CONTROL::init();
        cmd6->Bitfield.CommandStreamerStallEnable = true;
        cmd6->Bitfield.RenderTargetCacheFlushEnable = true;
        cmd6->Bitfield.DepthCacheFlushEnable = true;
        cmd6->Bitfield.DcFlushEnable = true;

        // Program VFE state
        auto cmd7 = commandStreamCSR->ptrOffset<MEDIA_VFE_STATE*>(sizeof(MEDIA_VFE_STATE));
        *cmd7 = MEDIA_VFE_STATE::init();
        cmd7->Bitfield.MaximumNumberOfThreads = this->maxVfeThreads - 1;
        cmd7->Bitfield.NumberOfUrbEntries = 0x1;
        cmd7->Bitfield.UrbEntryAllocationSize = 0x782;
        cmd7->Bitfield.PerThreadScratchSpace = this->perThreadScratchSpace;
        cmd7->Bitfield.StackSize = this->perThreadScratchSpace;
        size_t scratchSpaceOffset = 4096u;
        uint32_t lowAddress = static_cast<uint32_t>(0xffffffff & scratchSpaceOffset);
        uint32_t highAddress = static_cast<uint32_t>(0xffffffff & (scratchSpaceOffset >> 32));
        cmd7->Bitfield.ScratchSpaceBasePointer = lowAddress >> 0xa;
        cmd7->Bitfield.ScratchSpaceBasePointerHigh = highAddress;

        // Program register for preemption
        auto cmd8 = commandStreamCSR->ptrOffset<MI_LOAD_REGISTER_IMM*>(sizeof(MI_LOAD_REGISTER_IMM));
        uint32_t preemptMask = ((1 << 1) | (1 << 2)) << 16;
        uint32_t preemptValue = 0 | preemptMask;
        *cmd8 = MI_LOAD_REGISTER_IMM::init();
        cmd8->Bitfield.RegisterOffset = MMIOAddresses::preemptConfigReg >> 0x2;
        cmd8->Bitfield.DataDword = preemptValue;

        // Program Pipe Control
        auto cmd9 = commandStreamCSR->ptrOffset<PIPE_CONTROL*>(sizeof(PIPE_CONTROL));
        *cmd9 = PIPE_CONTROL::init();
        cmd9->Bitfield.CommandStreamerStallEnable = true;
        cmd9->Bitfield.TextureCacheInvalidationEnable = true;
        cmd9->Bitfield.DcFlushEnable = true;


        // Program State Base Address
        auto cmd10 = commandStreamCSR->ptrOffset<STATE_BASE_ADDRESS*>(sizeof(STATE_BASE_ADDRESS));
        *cmd10 = STATE_BASE_ADDRESS::init();

        cmd10->Bitfield.DynamicStateBaseAddressModifyEnable = true;
        cmd10->Bitfield.DynamicStateMemoryObjectControlState_IndexToMocsTables = MOCS::PageTableControlledCaching;
        cmd10->Bitfield.DynamicStateBaseAddress = dshAllocation->gpuAddress >> 0xc;
        cmd10->Bitfield.DynamicStateBufferSizeModifyEnable = true;
        cmd10->Bitfield.DynamicStateBufferSize = dshAllocation->size / MemoryConstants::pageSize;

        cmd10->Bitfield.SurfaceStateBaseAddressModifyEnable = true;
        cmd10->Bitfield.SurfaceStateMemoryObjectControlState_IndexToMocsTables = MOCS::PageTableControlledCaching;
        cmd10->Bitfield.SurfaceStateBaseAddress = sshAllocation->gpuAddress >> 0xc;

        cmd10->Bitfield.IndirectObjectBaseAddressModifyEnable = true;
        cmd10->Bitfield.IndirectObjectMemoryObjectControlState_IndexToMocsTables = MOCS::PageTableControlledCaching;
        cmd10->Bitfield.IndirectObjectBaseAddress = iohAllocation->gpuBaseAddress >> 0xc;
        cmd10->Bitfield.IndirectObjectBufferSizeModifyEnable = true;
        cmd10->Bitfield.IndirectObjectBufferSize = MemoryConstants::sizeOf4GBinPageEntities;

        cmd10->Bitfield.InstructionBaseAddressModifyEnable = true;
        cmd10->Bitfield.InstructionMemoryObjectControlState_IndexToMocsTables = MOCS::AggressiveCaching;
        cmd10->Bitfield.InstructionBaseAddress = decanonize(kernelAllocation->gpuBaseAddress) >> 0xc;
        cmd10->Bitfield.InstructionBufferSizeModifyEnable = true;
        cmd10->Bitfield.InstructionBufferSize = MemoryConstants::sizeOf4GBinPageEntities;

        if (scratchAllocation) {
            uint64_t gshAddress = scratchAllocation->gpuAddress - scratchSpaceOffset;
            cmd10->Bitfield.GeneralStateBaseAddressModifyEnable = true;
            cmd10->Bitfield.GeneralStateMemoryObjectControlState_IndexToMocsTables = MOCS::PageTableControlledCaching;
            cmd10->Bitfield.GeneralStateBaseAddress = decanonize(gshAddress) >> 0xc;
            cmd10->Bitfield.GeneralStateBufferSizeModifyEnable = true;
            cmd10->Bitfield.GeneralStateBufferSize = 0xfffff;
        }

        cmd10->Bitfield.StatelessDataPortAccessMemoryObjectControlState_IndexToMocsTables = MOCS::AggressiveCaching;

        cmd10->Bitfield.BindlessSurfaceStateBaseAddressModifyEnable = true;
        cmd10->Bitfield.BindlessSurfaceStateMemoryObjectControlState_IndexToMocsTables = MOCS::PageTableControlledCaching;
        cmd10->Bitfield.BindlessSurfaceStateBaseAddress = sshAllocation->gpuAddress >> 0xc;
        uint32_t bindlessSize = static_cast<uint32_t>((sshAllocation->size - MemoryConstants::pageSize) / 64) - 1;
        cmd10->Bitfield.BindlessSurfaceStateSize = bindlessSize;


        // Program System Instruction Pointer
        auto cmd11 = commandStreamCSR->ptrOffset<STATE_SIP*>(sizeof(STATE_SIP));
        *cmd11 = STATE_SIP::init();
        uint64_t sipAddressOffset = sipAllocation->gpuAddress - sipAllocation->gpuBaseAddress;
        cmd11->Bitfield.SystemInstructionPointer = sipAddressOffset >> 0x4;

        // Program Pipe Control
        auto cmd12 = commandStreamCSR->ptrOffset<PIPE_CONTROL*>(sizeof(PIPE_CONTROL));
        *cmd12 = PIPE_CONTROL::init();
        cmd12->Bitfield.CommandStreamerStallEnable = true;

        // Program MI_BATCH_BUFFER_START
        auto cmd13 = commandStreamCSR->ptrOffset<MI_BATCH_BUFFER_START*>(sizeof(MI_BATCH_BUFFER_START));
        *cmd13 = MI_BATCH_BUFFER_START::init();
        size_t taskOffset = commandStreamTask->offset - commandStreamTask->currentTaskOffset;
        auto taskAddress = ptrOffset(commandStreamTask->gpuAddress, taskOffset);
        cmd13->Bitfield.BatchBufferStartAddress_Graphicsaddress47_2 = taskAddress >> 0x2;
        //cmd13->Bitfield.BatchBufferStartAddress_Graphicsaddress47_2 = commandStreamTask->gpuAddress >> 0x2;
        cmd13->Bitfield.AddressSpaceIndicator = MI_BATCH_BUFFER_START::ADDRESS_SPACE_INDICATOR_PPGTT;

        alignToCacheLine(commandStreamCSR.get());
    }

    DBG_LOG("[DEBUG] BatchBuffer creation successful!\n");
    return SUCCESS;
}


void Context::alignToCacheLine(BufferObject* bo) {
    size_t used = bo->offset;
    void* pCmd = ptrOffset(bo->cpuAddress, used);
    size_t alignment = MemoryConstants::cacheLineSize;
    size_t partialCacheLine = used & (alignment - 1);
    if (partialCacheLine) {
        size_t amountToPad = alignment - partialCacheLine;
        memset(pCmd, 0x0, amountToPad);
        bo->offset += amountToPad;
        bo->currentTaskOffset += amountToPad;
    }
}


int Context::populateAndSubmitExecBuffer() {
    execBuffer = kernel->getExecData();
    execBuffer.push_back(kernelAllocation.get());
    if (scratchAllocation)
        execBuffer.push_back(scratchAllocation.get());
    execBuffer.push_back(dshAllocation.get());
    execBuffer.push_back(iohAllocation.get());
    execBuffer.push_back(sshAllocation.get());
    execBuffer.push_back(tagAllocation.get());
    execBuffer.push_back(preemptionAllocation.get());
    if (isMidThreadLevelPreemptionSupported) {
        execBuffer.push_back(sipAllocation.get());
    }
    execBuffer.push_back(commandStreamTask.get());
    size_t batchSize;
    size_t batchStartOffset;
    size_t residencyCount;
    if (taskCount == 1) {
        execBuffer.push_back(commandStreamCSR.get());
        batchSize = commandStreamCSR->currentTaskOffset;
        batchStartOffset = commandStreamCSR->offset - commandStreamCSR->currentTaskOffset;
        residencyCount = execBuffer.size();
        execObjects.resize(residencyCount);
    } else {
        batchSize = commandStreamTask->currentTaskOffset;
        batchStartOffset = commandStreamTask->offset - commandStreamTask->currentTaskOffset;
        residencyCount = execBuffer.size();
        execObjects.resize(residencyCount);
    }

    int ret = exec(execObjects.data(), execBuffer.data(), residencyCount, batchSize, batchStartOffset);
    if (ret) {
        execBuffer.clear();
        //TODO: What else must be done here?
        return GEM_EXECBUFFER_FAILED;
    }
    for (auto& bo : execBuffer) {
        bo->currentTaskOffset = 0u;
    }
    execBuffer.clear();
    //TODO: Reset offset values of BOs
    //TODO: Do ssh, dsh and ioh need an offset reset?

    return SUCCESS;
}


void Context::fillExecObject(drm_i915_gem_exec_object2& execObject, BufferObject* bo) {
    execObject.handle = bo->handle;
    execObject.relocation_count = 0u;
    execObject.relocs_ptr = 0ul;
    execObject.alignment = 0u;
    execObject.offset = bo->gpuAddress;
    execObject.flags = EXEC_OBJECT_PINNED | EXEC_OBJECT_SUPPORTS_48B_ADDRESS;
    execObject.rsvd1 = this->ctxId;
    execObject.rsvd2 = 0u;
}


int Context::exec(drm_i915_gem_exec_object2* execObjects, BufferObject** execBufferPtr, size_t residencyCount, size_t batchSize, size_t batchStartOffset) {
    for (size_t i = 0; i < residencyCount; i++) {
        fillExecObject(execObjects[i], execBufferPtr[i]);
    }
    drm_i915_gem_execbuffer2 execbuf = {0};
    execbuf.buffers_ptr = reinterpret_cast<uintptr_t>(execObjects);
    execbuf.buffer_count = static_cast<uint32_t>(residencyCount);
    execbuf.batch_start_offset = static_cast<uint32_t>(batchStartOffset);
    execbuf.batch_len = static_cast<uint32_t>(alignUp(batchSize, 8));
    execbuf.flags = I915_EXEC_RENDER | I915_EXEC_NO_RELOC;
    execbuf.rsvd1 = this->ctxId;

    int ret = ioctl(device->fd, DRM_IOCTL_I915_GEM_EXECBUFFER2, &execbuf);
    if (ret) {
        return GEM_EXECBUFFER_FAILED;
    }
    return SUCCESS;
}


int Context::finishExecution() {
    //execObjects.clear();
    //TODO: Clear execObjects vector here
    std::chrono::high_resolution_clock::time_point time1, time2;

    drm_i915_gem_wait wait = {0};
    wait.bo_handle = commandStreamCSR->handle;
    wait.timeout_ns = -1;
    time1 = std::chrono::high_resolution_clock::now();
    int ret = ioctl(device->fd, DRM_IOCTL_I915_GEM_WAIT, &wait);
    if (ret)
        return POST_SYNC_OPERATION_FAILED;
    time2 = std::chrono::high_resolution_clock::now();
    int64_t elapsedTime = std::chrono::duration_cast<std::chrono::nanoseconds>(time2 - time1).count();
    DBG_LOG("[DEBUG] GEM_WAIT time: %f seconds\n", elapsedTime/1e9);

    int64_t timeDiff = 0;
    int64_t timeoutMilliseconds = 200;
    time1 = std::chrono::high_resolution_clock::now();
    uint32_t* pollAddress = static_cast<uint32_t*>(tagAllocation->cpuAddress);
    while (*pollAddress != this->completionTag && timeDiff <= timeoutMilliseconds) {
        usleep(1);
        sched_yield();
        time2 = std::chrono::high_resolution_clock::now();
        timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time1).count();
    }
    DBG_LOG("[DEBUG] Timeout: %f seconds\n", timeDiff/1e3);
    DBG_LOG("[DEBUG] completionTag: %u\n", *pollAddress);
    if (*pollAddress != this->completionTag)
        return POST_SYNC_OPERATION_FAILED;

    return SUCCESS;
}






// DrmCommandStreamReceiver::flush() in drm_command_stream.inl
// DrmCommandStreamReceiver::flushInternal() in drm_command_stream_bdw_and_later.inl
// DrmCommandStreamReceiver::exec() in drm_command_stream.inl
// BufferObject::exec() in drm_buffer_object.cpp
//
// 0x7fffdd1ef000
// 0x7fffd95dd000
// 0x7fffd59cb000
// 0x8001fffea000  32Bit    0x555555cb3000
// 0x555556689000
// 0x555555c54000
// 0x8001fffda000  32Bit    0x555556207000
// 0x5555561f5000
// 0x5555556e4000
// 0x7ffff59c0000 
// 0x8001fffee000  32Bit    0x555555783000
// 0x5555557e0000
// 0x5555563f9000





// Commands in CommandStreamCSR:
// 1. PIPELINE_SELECT                    4
// 2. MI_LOAD_REGISTER_IMM              12
// 3. PIPE_CONTROL                      24
// 4. MI_LOAD_REGISTER_IMM              12
// 5. GPGPU_CSR_BASE_ADDRESS            12   64
// 6. PIPE_CONTROL                      24
// 7. MEDIA_VFE_STATE                   36  124
// 8. MI_LOAD_REGISTER_IMM              12  136
// 9. PIPE_CONTROL                      24  160
//10. STATE_BASE_ADDRESS                76  236
//11. STATE_SIP                         12  248
//12. PIPE_CONTROL                      24  272
//13. MI_BATCH_BUFFER_START             12  284
//14. CacheLineAlignment                    320

// Commands in CommandStreamTask:
// 1. MEDIA_STATE_FLUSH                  8
// 2. MEDIA_INTERFACE_DESCRIPTOR_LOAD   16
// 3. GPGPU_WALKER                      60
// 4. MEDIA_STATE_FLUSH                  8
// 5. PIPE_CONTROL                      48
// 6. MI_BATCH_BUFFER_END                4  144
// 7. CacheLineAlignment                    192








