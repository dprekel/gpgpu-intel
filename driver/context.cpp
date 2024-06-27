#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#include <memory>
#include <vector>

#include "commands_gen9.h"
#include "context.h"
#include "gpgpu.h"
#include "ioctl.h"
#include "utils.h"


Context::Context(Device* device) 
         : device(device),
           workItemsPerWorkGroup{1, 1, 1},
           globalWorkItems{1, 1, 1} {
    this->hwInfo = device->descriptor->pHwInfo;
    this->fclMain = device->fclMain;
    this->igcMain = device->igcMain;
    setMaxWorkGroupSize();
    setMaxThreadsForVfe();
}

Context::~Context() {
    printf("Context destructor called!\n");
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
    allocData.maxVfeThreads = hwInfo->gtSystemInfo->EUCount * numThreadsPerEU;
}

void Context::setKernelData(KernelFromPatchtokens* kernelData) {
    this->kernelData = kernelData;
}

KernelFromPatchtokens* Context::getKernelData() {
    return this->kernelData;
}

bool Context::getIsSipKernelAllocated() {
    return isSipKernelAllocated;
}


std::unique_ptr<BufferObject> Context::allocateBufferObject(size_t size) {
    size_t alignment = MemoryConstants::pageSize;
    //TODO: What is difference between this alignment and alignment in alignUp() function?
    size_t sizeToAlloc = size + alignment;
    void* pOriginalMemory = new (std::nothrow)char[sizeToAlloc];

    //TODO: Reformat the following if-else statement
    uintptr_t pAlignedMemory = reinterpret_cast<uintptr_t>(pOriginalMemory);
    if (pAlignedMemory) {
        pAlignedMemory += alignment;
        pAlignedMemory -= pAlignedMemory % alignment;
        reinterpret_cast<void**>(pAlignedMemory)[-1] = pOriginalMemory;
    } else {
        return nullptr;
    }
    drm_i915_gem_userptr userptr = {0};
    userptr.user_ptr = pAlignedMemory;
    userptr.user_size = size;
    userptr.flags = 0;

    int ret = ioctl(device->fd, DRM_IOCTL_I915_GEM_USERPTR, &userptr);
    if (ret) {
        return nullptr;
    }
    // add aligned free here
    auto bo = std::make_unique<BufferObject>();
    bo->cpuAddress = reinterpret_cast<void*>(pAlignedMemory);
    bo->gpuAddress = canonize(reinterpret_cast<uint64_t>(bo->cpuAddress));
    bo->size = size;
    bo->handle = userptr.handle;
    return bo;
}



int Context::createDrmContext() {
    int ret;
    vmId = device->drmVmId;
    
    drm_i915_gem_context_create_ext gcc = {0};
    ret = ioctl(device->fd, DRM_IOCTL_I915_GEM_CONTEXT_CREATE_EXT, &gcc);
    if (ret) {
        return CONTEXT_CREATION_FAILED;
    }
    if (vmId > 0) {
        drm_i915_gem_context_param param = {0};
        param.ctx_id = gcc.ctx_id;
        param.value = vmId;
        param.param = I915_CONTEXT_PARAM_VM;
        ret = ioctl(device->fd, DRM_IOCTL_I915_GEM_CONTEXT_SETPARAM, &param);
        if (ret) {
            return CONTEXT_CREATION_FAILED;
        }
    }
    ctxId = gcc.ctx_id;
    return SUCCESS;
}

void Context::setNonPersistentContext() {
    drm_i915_gem_context_param contextParam = {0};
    contextParam.ctx_id = ctxId;
    contextParam.param = I915_CONTEXT_PARAM_PERSISTENCE;
    ioctl(device->fd, DRM_IOCTL_I915_GEM_CONTEXT_SETPARAM, &contextParam);
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


int Context::createGPUAllocations() {
    int ret = 0;
    ret = allocateISAMemory();
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
        kernelAllocation = allocateBufferObject(alignedAllocationSize);
        if (!kernelAllocation) {
            return KERNEL_ALLOCATION_FAILED;
        }
        kernelAllocation->bufferType = BufferType::KERNEL_ISA;
    }
    memcpy(kernelAllocation->cpuAddress, kernelData->isa, kernelISASize);
    return SUCCESS;
}


//TODO: Check why Intels GEMM uses no Scratch Space
//TODO: Check necessary alignment size
//TODO: Make sure that mediaVfeState[0] is never nullptr
//TODO: Unify BUFFER_ALLOCATION_FAILED like macros
int Context::createScratchAllocation() {
    uint32_t computeUnitsUsedForScratch = hwInfo->gtSystemInfo->MaxSubSlicesSupported
                                        * hwInfo->gtSystemInfo->MaxEuPerSubSlice
                                        * hwInfo->gtSystemInfo->ThreadCount
                                        / hwInfo->gtSystemInfo->EUCount;
    uint32_t requiredScratchSize = kernelData->mediaVfeState[0]->PerThreadScratchSpace;
    size_t requiredScratchSizeInBytes = requiredScratchSize * computeUnitsUsedForScratch;
    if (requiredScratchSize) {
        size_t alignedAllocationSize = alignUp(requiredScratchSizeInBytes, MemoryConstants::pageSize);
        scratchAllocation = allocateBufferObject(alignedAllocationSize);
        if (!scratchAllocation)
            return BUFFER_ALLOCATION_FAILED;
        scratchAllocation->bufferType = BufferType::SCRATCH_SURFACE;
        allocData.scratchAddress = scratchAllocation->gpuAddress;
        requiredScratchSize >>= static_cast<uint32_t>(MemoryConstants::kiloByteShiftSize);
        allocData.perThreadScratchSpace = 0u;
        while (requiredScratchSize >>= 1) {
            allocData.perThreadScratchSpace++;
        }
        return SUCCESS;
    }
    allocData.scratchAddress = 0u;
    allocData.perThreadScratchSpace = 0u;
    return SUCCESS;
}


int Context::createSurfaceStateHeap() {
    size_t sshSize = static_cast<size_t>(kernelData->header->SurfaceStateHeapSize);
    if (!sshAllocation) {
        size_t alignedAllocationSize = alignUp(sshSize, MemoryConstants::pageSize); //TODO: this is not right size
        sshAllocation = allocateBufferObject(alignedAllocationSize);
        if (!sshAllocation)
            return BUFFER_ALLOCATION_FAILED;
        sshAllocation->bufferType = BufferType::LINEAR_STREAM;
    }
    if (kernelData->bindingTableState->Count == 0)
        return 0;
    char* srcSsh = kernel->getSurfaceStatePtr();
    memcpy(sshAllocation->cpuAddress, srcSsh, sshSize);
    return SUCCESS;
}


int Context::createIndirectObjectHeap() {
    /*
    size_t localWorkSize = 16;            // from clEnqueueNDRangeKernel argument
    uint8_t numChannels = 3;             // from kernel.kernelInfo.kernelDescriptor.kernelAttributes.numLocalIdChannels
    uint32_t grfSize = 32;                // from sizeof(typename GfxFamily::GRF)
    uint32_t crossThreadDataSize = 96;    // from kernel.kernelInfo.kernelDescriptor.kernelAttributes.crossThreadDataSize
    uint32_t simdSize = 16;               // from kernel.kernelInfo.kernelDescriptor.kernelAttributes.simdSize

    uint32_t numGRFsPerThread = (simdSize == 32 && grfSize == 32) ? 2 : 1;
    uint32_t perThreadSizeLocalIDs = numGRFsPerThread * grfSize * (simdSize == 1 ? 1u : numChannels);
    perThreadSizeLocalIDs = std::max(perThreadSizeLocalIDs, grfSize);
    uint64_t threadsPerWG = simdSize + localWorkSize - 1;
    threadsPerWG >>= simdSize == 32 ? 5 : simdSize == 16 ? 4 : simdSize == 8 ? 3 : 0;

    // in matmul test example, size will be 192
    uint64_t size = crossThreadDataSize + threadsPerWG * perThreadSizeLocalIDs;
    */
    if (!iohAllocation) {
        size_t iohSize = 16 * MemoryConstants::pageSize;
        iohAllocation = allocateBufferObject(iohSize);
        if (!iohAllocation)
            return BUFFER_ALLOCATION_FAILED;
        iohAllocation->bufferType = BufferType::INTERNAL_HEAP;
    }
    //TODO: Terminate program if we have implicitArgs
    char* crossThreadData = kernel->getCrossThreadData();
    uint32_t* patchPtr;
    uint32_t patchOffset;
    for (uint32_t i = 0; i < 3; i++) {
        patchOffset = kernelData->crossThreadPayload.localWorkSize[i]->Offset;
        patchPtr = reinterpret_cast<uint32_t*>(ptrOffset(crossThreadData, patchOffset));
        *patchPtr = static_cast<uint32_t>(workItemsPerWorkGroup[i]);
        if (kernelData->crossThreadPayload.globalWorkSize[i]) {
            patchOffset = kernelData->crossThreadPayload.globalWorkSize[i]->Offset;
            patchPtr = reinterpret_cast<uint32_t*>(ptrOffset(crossThreadData, patchOffset));
            *patchPtr = static_cast<uint32_t>(globalWorkItems[i]);
        }
    }
    
    uint32_t crossThreadDataSize = kernelData->dataParameterStream->DataParameterStreamSize;
    memcpy(iohAllocation->cpuAddress, crossThreadData, crossThreadDataSize);

    size_t localWorkSize = workItemsPerWorkGroup[0] * workItemsPerWorkGroup[1] * workItemsPerWorkGroup[2];
    uint32_t simdSize = kernelData->executionEnvironment->LargestCompiledSIMDSize;
    uint64_t threadsPerWG = simdSize + localWorkSize - 1;
    threadsPerWG >>= simdSize == 32 ? 5 : simdSize == 16 ? 4 : simdSize == 8 ? 3 : 0;
    allocData.hwThreadsPerWorkGroup = threadsPerWG;

    //generateLocalIDsSimd(iohAllocation->cpuAddress, threadsPerWG, simdSize);

    return SUCCESS;
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
    //TODO: Why 16 times pageSize?
    if (!dshAllocation) {
        size_t dshSize = 16 * MemoryConstants::pageSize;
        dshAllocation = allocateBufferObject(dshSize);
        if (!dshAllocation)
            return BUFFER_ALLOCATION_FAILED;
        dshAllocation->bufferType = BufferType::LINEAR_STREAM;
    }
    auto interfaceDescriptor = dshAllocation->ptrOffset<INTERFACE_DESCRIPTOR_DATA*>(sizeof(INTERFACE_DESCRIPTOR_DATA));
    *interfaceDescriptor = INTERFACE_DESCRIPTOR_DATA::init();

    uint64_t kernelStartOffset = reinterpret_cast<uint64_t>(kernelAllocation->cpuAddress);
    interfaceDescriptor->Bitfield.KernelStartPointerHigh = kernelStartOffset >> 32;
    interfaceDescriptor->Bitfield.KernelStartPointer = (uint32_t)kernelStartOffset >> 0x6;
    interfaceDescriptor->Bitfield.DenormMode = INTERFACE_DESCRIPTOR_DATA::DENORM_MODE_SETBYKERNEL;
    interfaceDescriptor->Bitfield.BindingTablePointer = static_cast<uint32_t>(kernelData->bindingTableState->Offset) >> 0x5;
    interfaceDescriptor->Bitfield.SharedLocalMemorySize = 0u;
    interfaceDescriptor->Bitfield.NumberOfThreadsInGpgpuThreadGroup = static_cast<uint32_t>(allocData.hwThreadsPerWorkGroup);
    // one general purpose register file (GRF) has 32 bytes on GEN9
    uint32_t grfSize = 32;
    size_t crossThreadDataSize = kernelData->dataParameterStream->DataParameterStreamSize;
    interfaceDescriptor->Bitfield.CrossThreadConstantDataReadLength = static_cast<uint32_t>(crossThreadDataSize / grfSize);
    //TODO: Retrieve perThreadDataSize from ioh
    size_t perThreadDataSize = 60;
    uint32_t numGrfPerThreadData = static_cast<uint32_t>(perThreadDataSize / grfSize);
    numGrfPerThreadData = std::max(numGrfPerThreadData, 1u);
    interfaceDescriptor->Bitfield.ConstantIndirectUrbEntryReadLength = numGrfPerThreadData;
    interfaceDescriptor->Bitfield.BarrierEnable = kernelData->executionEnvironment->HasBarriers;
    return SUCCESS;
}


int Context::createPreemptionAllocation() {
    size_t preemptionSize = hwInfo->gtSystemInfo->CsrSizeInMb * MemoryConstants::megaByte;
    preemptionAllocation = allocateBufferObject(preemptionSize);
    if (!preemptionAllocation) {
        return BUFFER_ALLOCATION_FAILED;
    }
    preemptionAllocation->bufferType = BufferType::PREEMPTION;
    return SUCCESS;
}


int Context::createTagAllocation() {
    tagAllocation = allocateBufferObject(MemoryConstants::pageSize);
    if (!tagAllocation) {
        return BUFFER_ALLOCATION_FAILED;
    }
    tagAllocation->bufferType = BufferType::TAG_BUFFER;
    uint32_t* tagAddress = reinterpret_cast<uint32_t*>(tagAllocation->cpuAddress);
    uint32_t initialHardwareTag = 0u;
    *tagAddress = initialHardwareTag;

    uint8_t* tagAdd = reinterpret_cast<uint8_t*>(tagAllocation->cpuAddress);
    DebugPauseState* debugPauseStateAddress = reinterpret_cast<DebugPauseState*>(tagAdd + MemoryConstants::cacheLineSize);
    *debugPauseStateAddress = DebugPauseState::waitingForFirstSemaphore;
    return SUCCESS;
}


int Context::createSipAllocation(size_t sipSize, const char* sipBinaryRaw) {
    size_t sipAllocSize = alignUp(sipSize, MemoryConstants::pageSize);
    sipAllocation = allocateBufferObject(sipAllocSize);
    if (!sipAllocation)
        return BUFFER_ALLOCATION_FAILED;
    sipAllocation->bufferType = BufferType::KERNEL_ISA_INTERNAL;
    memcpy(sipAllocation->cpuAddress, sipBinaryRaw, sipSize);
    isSipKernelAllocated = true;
    return SUCCESS;
}


int Context::createCommandStreamTask() {
    //TODO: Reset offset value if EnqueueNDRangeKernel is called multiple times
    //TODO: Always 16 * pageSize?
    if (!commandStreamTask) {
        commandStreamTask = allocateBufferObject(16 * MemoryConstants::pageSize);
        if (!commandStreamTask) {
            return BUFFER_ALLOCATION_FAILED;
        }
        commandStreamTask->bufferType = BufferType::COMMAND_BUFFER;
    }
    auto cmd1 = commandStreamTask->ptrOffset<MEDIA_STATE_FLUSH*>(sizeof(MEDIA_STATE_FLUSH));
    *cmd1 = MEDIA_STATE_FLUSH::init();

    auto cmd2 = commandStreamTask->ptrOffset<MEDIA_INTERFACE_DESCRIPTOR_LOAD*>(sizeof(MEDIA_INTERFACE_DESCRIPTOR_LOAD));
    *cmd2 = MEDIA_INTERFACE_DESCRIPTOR_LOAD::init();
    cmd2->Bitfield.InterfaceDescriptorDataStartAddress = 0u;
    cmd2->Bitfield.InterfaceDescriptorTotalLength = sizeof(INTERFACE_DESCRIPTOR_DATA);

    // we need a variable that stores the preemption mode we are using
    // we don't need to dispatch workarounds on Skylake, but maybe on other architectures
    auto cmd3 = commandStreamTask->ptrOffset<GPGPU_WALKER*>(sizeof(GPGPU_WALKER));
    *cmd3 = GPGPU_WALKER::init();
    /*
    cmd3->Bitfield.IndirectDataStartAddress = static_cast<uint32_t>(offsetCrossThreadData);
    cmd3->Bitfield.InterfaceDescriptorOffset = interfaceDescriptorIndex;
    cmd3->Bitfield.IndirectDataLength = indirectDataLength;
    */
    cmd3->Bitfield.ThreadWidthCounterMaximum = static_cast<uint32_t>(allocData.hwThreadsPerWorkGroup);
    cmd3->Bitfield.ThreadGroupIdXDimension = static_cast<uint32_t>(numWorkGroups[0]);
    cmd3->Bitfield.ThreadGroupIdYDimension = static_cast<uint32_t>(numWorkGroups[1]);
    cmd3->Bitfield.ThreadGroupIdZDimension = static_cast<uint32_t>(numWorkGroups[2]);
    size_t localWorkSize = workItemsPerWorkGroup[0] * workItemsPerWorkGroup[1] * workItemsPerWorkGroup[2];
    uint32_t simdSize = kernelData->executionEnvironment->LargestCompiledSIMDSize;
    auto remainderSimdLanes = localWorkSize & (simdSize - 1);
    uint64_t executionMask = maxNBitValue(remainderSimdLanes);
    cmd3->Bitfield.RightExecutionMask = static_cast<uint32_t>(executionMask);
    cmd3->Bitfield.BottomExecutionMask = static_cast<uint32_t>(0xffffffff);
    cmd3->Bitfield.SimdSize = (simdSize == 1) ? (32 >> 4) : (simdSize >> 4);
    cmd3->Bitfield.ThreadGroupIdStartingX = 0u;
    cmd3->Bitfield.ThreadGroupIdStartingY = 0u;
    cmd3->Bitfield.ThreadGroupIdStartingResumeZ = 0u;

    auto cmd4 = commandStreamTask->ptrOffset<MEDIA_STATE_FLUSH*>(sizeof(MEDIA_STATE_FLUSH));
    *cmd4 = MEDIA_STATE_FLUSH::init();
    //TODO: interfaceDescriptorIndex: What is this?
    //cmd4->Bitfield.InterfaceDescriptorOffset = interfaceDescriptorIndex;

    // updateTag, dispatchFlags.guardCommandBufferWithPipeControl (true), 
    // dispatchFlags.memoryMigrationRequired (migratableArgsMap), activePartitions, staticWorkPartitioningEnabled

    // This driver doesn't support command queues with batched submission. So each 
    // kernel submission needs its own pipe control. A batched submission would only 
    // need one pipe control for the whole batch.
    auto cmd5 = commandStreamTask->ptrOffset<PIPE_CONTROL*>(sizeof(PIPE_CONTROL));
    *cmd5 = PIPE_CONTROL::init();
    cmd5->Bitfield.CommandStreamerStallEnable = true;
    cmd5->Bitfield.ConstantCacheInvalidationEnable = false;
    cmd5->Bitfield.InstructionCacheInvalidateEnable = false;
    cmd5->Bitfield.PipeControlFlushEnable = false;
    cmd5->Bitfield.RenderTargetCacheFlushEnable = false;
    cmd5->Bitfield.StateCacheInvalidationEnable = false;
    cmd5->Bitfield.TextureCacheInvalidationEnable = false;
    cmd5->Bitfield.VfCacheInvalidationEnable = false;
    cmd5->Bitfield.GenericMediaStateClear = false;
    //TODO: TlbInvalidate false or true?
    //cmd5->Bitfield.TlbInvalidate = ;

    // has something to do with debugging, is set in DrmCommandStreamReceiver constructor
    // (drm_command_stream.inl)
    cmd5->Bitfield.NotifyEnable = true;

    // For this driver, DcFlushEnable is always true. If batched submission would be
    // possible with this driver, DcFlushEnable would be true if:
    // - capabilityTable.gpuAddressSpace >= maxNBitValue(47) (maybe true on some 
    //   architectures?)
    // - isFlushL3Required == true (seems to be always the case)
    cmd5->Bitfield.DcFlushEnable = true;

    cmd5->Bitfield.PostSyncOperation = PIPE_CONTROL::POST_SYNC_OPERATION_WRITE_IMMEDIATE_DATA;
    uint64_t tagAddress = tagAllocation->gpuAddress;
    cmd5->Bitfield.Address = static_cast<uint32_t>(tagAddress & 0x0000ffffffffull);
    cmd5->Bitfield.AddressHigh = static_cast<uint32_t>(tagAddress >> 32);

    // immediateData == taskCount (always 1 if no batched submission)
    cmd5->Bitfield.ImmediateData = 1u;

    auto cmd6 = commandStreamTask->ptrOffset<MI_BATCH_BUFFER_START*>(sizeof(MI_BATCH_BUFFER_START));
    *cmd6 = MI_BATCH_BUFFER_START::init();
    cmd6->Bitfield.BatchBufferStartAddress_Graphicsaddress47_2 = 0u;
    cmd6->Bitfield.AddressSpaceIndicator = MI_BATCH_BUFFER_START::ADDRESS_SPACE_INDICATOR_PPGTT;

    //TODO: Add noop
    //TODO: align to cache line size

    allocData.commandStreamTaskAddress = commandStreamTask->gpuAddress;
    return SUCCESS;
}





int Context::createCommandStreamReceiver() {
    //TODO: Reset offset value if EnqueueNDRangeKernel is called multiple times
    //TODO: Always 16 * pageSize?
    if (!commandStreamCSR) {
        commandStreamCSR = allocateBufferObject(16 * MemoryConstants::pageSize);
        if (!commandStreamCSR) {
            return BUFFER_ALLOCATION_FAILED;
        }
        commandStreamCSR->bufferType = BufferType::COMMAND_BUFFER;
    }
    //TODO: program Pipeline Select
    //if (mediaSamplerConfigChanged || !isPreambleSent) {
        auto cmd1 = commandStreamCSR->ptrOffset<PIPELINE_SELECT*>(sizeof(PIPELINE_SELECT));
        *cmd1 = PIPELINE_SELECT::init();
        //cmd1->Bitfield.MaskBits = mask;
        cmd1->Bitfield.PipelineSelection = PIPELINE_SELECT::PIPELINE_SELECTION_GPGPU;
        //cmd1->Bitfield.MediaSamplerDopClockGateEnable = !pipelineSelectArgs.mediaSamplerRequired;
    //}

    // program Preamble:
    // program L3 cache:
    auto cmd2 = commandStreamCSR->ptrOffset<MI_LOAD_REGISTER_IMM*>(sizeof(MI_LOAD_REGISTER_IMM));
    *cmd2 = MI_LOAD_REGISTER_IMM::init();
    uint32_t L3RegisterOffset = 0x7034;
    uint32_t L3ValueNoSLM = 0x80000340;
    cmd2->Bitfield.RegisterOffset = L3RegisterOffset;
    cmd2->Bitfield.DataDword = L3ValueNoSLM;

    // program Thread Arbitration
    auto cmd3 = commandStreamCSR->ptrOffset<PIPE_CONTROL*>(sizeof(PIPE_CONTROL));
    *cmd3 = PIPE_CONTROL::init();
    cmd3->Bitfield.CommandStreamerStallEnable = true;

    auto cmd4 = commandStreamCSR->ptrOffset<MI_LOAD_REGISTER_IMM*>(sizeof(MI_LOAD_REGISTER_IMM));
    *cmd4 = MI_LOAD_REGISTER_IMM::init();
    uint32_t debugControlReg2Offset = 0xe404;
    uint32_t requiredThreadArbitrationPolicy = ThreadArbitrationPolicy::RoundRobin;
    cmd4->Bitfield.RegisterOffset = debugControlReg2Offset;
    cmd4->Bitfield.DataDword = requiredThreadArbitrationPolicy;

    // program Preemption
    //if (isMidThreadPreemption) {
        auto cmd5 = commandStreamCSR->ptrOffset<GPGPU_CSR_BASE_ADDRESS*>(sizeof(GPGPU_CSR_BASE_ADDRESS));
        *cmd5 = GPGPU_CSR_BASE_ADDRESS::init();
        cmd5->Bitfield.GpgpuCsrBaseAddress = preemptionAllocation->gpuAddress;
    //}

    auto cmd6 = commandStreamCSR->ptrOffset<PIPE_CONTROL*>(sizeof(PIPE_CONTROL));
    *cmd6 = PIPE_CONTROL::init();
    cmd6->Bitfield.CommandStreamerStallEnable = true;
    //if (waSendMIFLUSHBeforeVFE) {
        cmd6->Bitfield.RenderTargetCacheFlushEnable = true;
        cmd6->Bitfield.DepthCacheFlushEnable = true;
        cmd6->Bitfield.DcFlushEnable = true;
    //}
    // program VFE state
    //if (mediaVfeStateDirty) {
        auto cmd7 = commandStreamCSR->ptrOffset<MEDIA_VFE_STATE*>(sizeof(MEDIA_VFE_STATE));
        *cmd7 = MEDIA_VFE_STATE::init();
        cmd7->Bitfield.MaximumNumberOfThreads = allocData.maxVfeThreads;
        cmd7->Bitfield.NumberOfUrbEntries = 1;
        cmd7->Bitfield.UrbEntryAllocationSize = 0x782;
        cmd7->Bitfield.PerThreadScratchSpace = allocData.perThreadScratchSpace;
        cmd7->Bitfield.StackSize = allocData.perThreadScratchSpace;
        uint32_t lowAddress = static_cast<uint32_t>(0xffffffff & scratchAllocation->gpuAddress);
        uint32_t highAddress = static_cast<uint32_t>(0xffffffff & (scratchAllocation->gpuAddress >> 32));
        cmd7->Bitfield.ScratchSpaceBasePointer = lowAddress;
        cmd7->Bitfield.ScratchSpaceBasePointerHigh = highAddress;
    //}

    //TODO: program Preemption again?
    auto cmd8 = commandStreamCSR->ptrOffset<GPGPU_CSR_BASE_ADDRESS*>(sizeof(GPGPU_CSR_BASE_ADDRESS));
    *cmd8 = GPGPU_CSR_BASE_ADDRESS::init();

    // Program State Base Address
    // 1. Pipe Control
    // 2. Additional Pipeline Select (if 3DPipelineSelectWARequired)
    // 3. Program State Base Address
    // 4. Additional Pipeline Select (if 3DPipelineSelectWARequired)
    // 5. Program Sip State

    //TODO: This Pipe Control
    auto cmd9 = commandStreamCSR->ptrOffset<PIPE_CONTROL*>(sizeof(PIPE_CONTROL));
    *cmd9 = PIPE_CONTROL::init();
    cmd9->Bitfield.CommandStreamerStallEnable = true;
    cmd9->Bitfield.DcFlushEnable = true;

    //TODO: Program State Base Address
    auto cmd10 = commandStreamCSR->ptrOffset<STATE_BASE_ADDRESS*>(sizeof(STATE_BASE_ADDRESS));
    *cmd10 = STATE_BASE_ADDRESS::init();

    cmd10->Bitfield.DynamicStateBaseAddressModifyEnable = true;
    cmd10->Bitfield.DynamicStateBufferSizeModifyEnable = true;
    cmd10->Bitfield.DynamicStateBaseAddress = dshAllocation->gpuAddress;
    cmd10->Bitfield.DynamicStateBufferSize = dshAllocation->size;

    cmd10->Bitfield.SurfaceStateBaseAddressModifyEnable = true;
    cmd10->Bitfield.SurfaceStateBaseAddress = sshAllocation->gpuAddress;

    cmd10->Bitfield.IndirectObjectBaseAddressModifyEnable = true;
    cmd10->Bitfield.IndirectObjectBufferSizeModifyEnable = true;
    cmd10->Bitfield.IndirectObjectBaseAddress = iohAllocation->gpuAddress;
    cmd10->Bitfield.IndirectObjectBufferSize = iohAllocation->size;

    cmd10->Bitfield.InstructionBaseAddressModifyEnable = true;
    cmd10->Bitfield.InstructionBufferSizeModifyEnable = true;
    cmd10->Bitfield.InstructionBaseAddress = kernelAllocation->gpuAddress;
    //cmd10->Bitfield.InstructionBufferSize = MemoryConstants::sizeOf4GBinPageEntities;
    //cmd10->Bitfield.InstructionMemoryObjectControlState = getMOCS();

    cmd10->Bitfield.GeneralStateBaseAddressModifyEnable = true;
    cmd10->Bitfield.GeneralStateBufferSizeModifyEnable = true;
    //cmd10->Bitfield.GeneralStateBaseAddress = allocData.gshAddress;
    cmd10->Bitfield.GeneralStateBufferSize = 0xfffff;

    //cmd10->Bitfield.StatelessDataPortAccessMemoryObjectControlState = statelessMocsIndex;
    //TODO: Add missing fields

    //TODO: Program State Sip
    //if (isMidThreadPreemption) {
        auto cmd11 = commandStreamCSR->ptrOffset<STATE_SIP*>(sizeof(STATE_SIP));
        *cmd11 = STATE_SIP::init();
        cmd11->Bitfield.SystemInstructionPointer = sipAllocation->gpuAddress;
    //}

    //TODO: Program Pipe Control
    auto cmd12 = commandStreamCSR->ptrOffset<PIPE_CONTROL*>(sizeof(PIPE_CONTROL));
    *cmd12 = PIPE_CONTROL::init();
    cmd12->Bitfield.CommandStreamerStallEnable = true;
    cmd12->Bitfield.DcFlushEnable = true;

    auto cmd13 = commandStreamCSR->ptrOffset<MI_BATCH_BUFFER_START*>(sizeof(MI_BATCH_BUFFER_START));
    *cmd13 = MI_BATCH_BUFFER_START::init();
    cmd13->Bitfield.BatchBufferStartAddress_Graphicsaddress47_2 = commandStreamTask->gpuAddress;
    cmd13->Bitfield.AddressSpaceIndicator = MI_BATCH_BUFFER_START::ADDRESS_SPACE_INDICATOR_PPGTT;
    //TODO: align to cache line size

    return SUCCESS;
}



int Context::populateAndSubmitExecBuffer() {
    //TODO: Don't copy the objects, instead use raw pointers
    execBuffer.push_back(kernelAllocation.get());
    if (scratchAllocation)
        execBuffer.push_back(scratchAllocation.get());
    execBuffer.push_back(dshAllocation.get());
    execBuffer.push_back(iohAllocation.get());
    execBuffer.push_back(sshAllocation.get());
    execBuffer.push_back(tagAllocation.get());
    //if (midThreadPreemption) {
        execBuffer.push_back(preemptionAllocation.get());
        execBuffer.push_back(sipAllocation.get());
    //}
    execBuffer.push_back(commandStreamTask.get());
    execBuffer.push_back(commandStreamCSR.get());

    size_t residencyCount = execBuffer.size();
    execObjects.resize(residencyCount);

    //TODO: Calculate used variable
    size_t used = 320;
    int ret = exec(execObjects.data(), execBuffer.data(), residencyCount, used);
    if (ret)
        return GEM_EXECBUFFER_FAILED;
    execBuffer.clear();

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
    // DrmCommandStreamReceiver::flush() in drm_command_stream.inl
    // DrmCommandStreamReceiver::flushInternal() in drm_command_stream_bdw_and_later.inl
    // DrmCommandStreamReceiver::exec() in drm_command_stream.inl
    // BufferObject::exec() in drm_buffer_object.cpp


int Context::exec(drm_i915_gem_exec_object2* execObjects, BufferObject** execBufferPtr, size_t residencyCount, size_t used) {
    for (size_t i = 0; i < residencyCount; i++) {
        fillExecObject(execObjects[i], execBufferPtr[i]);
    }
    drm_i915_gem_execbuffer2 execbuf = {0};
    execbuf.buffers_ptr = reinterpret_cast<uintptr_t>(execObjects);
    execbuf.buffer_count = static_cast<uint32_t>(residencyCount);
    execbuf.batch_start_offset = 0u;
    //TODO: Add calculation of used variable to ptrOffset() function
    //TODO: Cast alignUp() result?
    execbuf.batch_len = alignUp(used, 8);
    execbuf.flags = I915_EXEC_RENDER | I915_EXEC_NO_RELOC;
    execbuf.rsvd1 = this->ctxId;

    /*
    int ret = ioctl(device->fd, DRM_IOCTL_I915_GEM_EXECBUFFER2, &execbuf);
    if (ret) {
        return GEM_EXECBUFFER_FAILED;
    }
    */
    return SUCCESS;
}


int Context::finishExecution() {
    //TODO: Clear execObjects vector here
    return SUCCESS;
}





// Commands in CommandStreamCSR:
// 1. PIPELINE_SELECT                    4
// 2. MI_LOAD_REGISTER_IMM              12
// 3. PIPE_CONTROL                      24
// 4. MI_LOAD_REGISTER_IMM              12
// 5. GPGPU_CSR_BASE_ADDRESS            12   64
// 6. PIPE_CONTROL                      24
// 7. MEDIA_VFE_STATE                   36  124
// 8. GPGPU_CSR_BASE_ADDRESS            12  136
// 9. PIPE_CONTROL                      24  160
//10. STATE_BASE_ADDRESS                76  236
//11. STATE_SIP                         12  248
//12. PIPE_CONTROL                      24  272
//13. MI_BATCH_BUFFER_START             12  284



// Commands in CommandStreamTask:
// 1. MEDIA_STATE_FLUSH
// 2. MEDIA_INTERFACE_DESCRIPTOR_LOAD
// 3. GPGPU_WALKER
// 4. MEDIA_STATE_FLUSH
// 5. PIPE_CONTROL
// 6. MI_BATCH_BUFFER_START
// 7. NOOP








