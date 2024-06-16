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

#define PAGE_SIZE 4096


Context::Context(Device* device) 
         : device(device),
           workItemsPerWorkGroup{1, 1, 1},
           globalWorkItems{1, 1, 1} {
    this->hwInfo = device->descriptor->pHwInfo;
    setMaxWorkGroupSize();
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

void Context::setKernelData(KernelFromPatchtokens* kernelData) {
    this->kernelData = kernelData;
}

KernelFromPatchtokens* Context::getKernelData() {
    return this->kernelData;
}

BufferObject* Context::allocateBufferObject(size_t size) {
    size_t alignment = PAGE_SIZE;
    //TODO: What is difference between this alignment and alignment in alignUp() function?
    size_t sizeToAlloc = size + alignment;
    //TODO: Replace malloc with new
    void* pOriginalMemory = malloc(sizeToAlloc);

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
    //TODO: Should we calculate GPU address already here?
    bo->gpuAddress = canonize(reinterpret_cast<uint64_t>(bo->cpuAddress));
    bo->size = size;
    bo->handle = userptr.handle;
    auto BO = bo.get();
    //TODO: There is a problem with std::move here
    execBuffer.push_back(std::move(bo));
    return BO;
}

/*
int Context::emitPinningRequest(BufferObject* bo) {
    drm_i915_gem_exec_object2 execObject = {};
    execObject.handle = bo->handle;
    execObject.relocations_count = 0;
    execObject.relocs_ptr = 0ul;
    execObject.alignment = 0;
    execObject.offset = bo->cpuAddress;
    execObject.flags = EXEC_OBJECT_PINNED | EXEC_OBJECT_SUPPORTS_48B_ADDRESS;
    execObject.rsvd1 = this->ctxId;
    execObject.rsvd2 = 0;

    drm_i915_gem_execbuffer2 execbuf = {};
    execbuf.buffers_ptr = reinterpret_cast<uintptr_t>(&execObject);
    execbuf.buffer_count = 2u;
    execbuf.batch_start_offset = 0u;
    execbuf.batch_len = alignUp(used, 8);
    execbuf.flags = flags;
    execbuf.rsvd1 = this->ctxId;

    int ret = ioctl(fd, DRM_IOCTL_I915_GEM_EXECBUFFER2, &execbuf);
    if (ret) {
        return -1;
    }
    return 0;
}
*/


int Context::createDrmContext() {
    int ret;
    vmId = device->drmVmId;
    
    drm_i915_gem_context_create_ext gcc = {};
    ret = ioctl(device->fd, DRM_IOCTL_I915_GEM_CONTEXT_CREATE_EXT, &gcc);
    if (ret) {
        return CONTEXT_CREATION_FAILED;
    }
    if (vmId > 0) {
        drm_i915_gem_context_param param = {};
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
    drm_i915_gem_context_param contextParam = {};
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
    return SUCCESS;
}


int Context::allocateISAMemory() {
    size_t kernelISASize = static_cast<size_t>(kernelData->header->KernelHeapSize);
    size_t alignedAllocationSize = alignUp(kernelISASize, PAGE_SIZE);
    BufferObject* kernelISA = allocateBufferObject(alignedAllocationSize);
    if (!kernelISA) {
        return KERNEL_ALLOCATION_FAILED;
    }
    kernelISA->bufferType = BufferType::KERNEL_ISA;
    memcpy(kernelISA->cpuAddress, kernelData->isa, kernelISASize);
    allocData.kernelAddr = kernelISA->gpuAddress;
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
        size_t alignedAllocationSize = alignUp(requiredScratchSizeInBytes, PAGE_SIZE);
        BufferObject* scratch = allocateBufferObject(alignedAllocationSize);
        if (!scratch)
            return BUFFER_ALLOCATION_FAILED;
        scratch->bufferType = BufferType::SCRATCH_SURFACE;
    }
    return SUCCESS;
}


int Context::createSurfaceStateHeap() {
    size_t sshSize = static_cast<size_t>(kernelData->header->SurfaceStateHeapSize);
    size_t alignedAllocationSize = alignUp(sshSize, PAGE_SIZE); //TODO: this is not right size
    BufferObject* dstSsh = allocateBufferObject(alignedAllocationSize);
    if (!dstSsh)
        return BUFFER_ALLOCATION_FAILED;
    dstSsh->bufferType = BufferType::LINEAR_STREAM;
    if (kernelData->bindingTableState->Count == 0)
        return 0;
    auto srcSsh = kernel->getSurfaceStatePtr();
    memcpy(dstSsh->cpuAddress, srcSsh, sshSize);
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
    size_t iohSize = 16 * PAGE_SIZE;
    BufferObject* ioh = allocateBufferObject(iohSize);
    if (!ioh)
        return BUFFER_ALLOCATION_FAILED;
    ioh->bufferType = BufferType::INTERNAL_HEAP;
    
    //TODO: Terminate program if we have implicitArgs
    uint32_t crossThreadDataSize = kernelData->dataParameterStream->DataParameterStreamSize;
    memset(ioh->cpuAddress, 0x00, crossThreadDataSize);

    size_t localWorkSize = 16; //TODO: Calculate from arguments
    uint32_t simdSize = kernelData->executionEnvironment->LargestCompiledSIMDSize;
    uint64_t threadsPerWG = simdSize + localWorkSize - 1;
    threadsPerWG >>= simdSize == 32 ? 5 : simdSize == 16 ? 4 : simdSize == 8 ? 3 : 0;
    generateLocalIDsSimd(ioh->cpuAddress, threadsPerWG, simdSize);


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
    size_t dshSize = 16 * PAGE_SIZE;
    BufferObject* dsh = allocateBufferObject(dshSize);
    if (!dsh)
        return BUFFER_ALLOCATION_FAILED;
    dsh->bufferType = BufferType::LINEAR_STREAM;

    auto interfaceDescriptor = dsh->ptrOffset<INTERFACE_DESCRIPTOR_DATA*>(sizeof(INTERFACE_DESCRIPTOR_DATA));
    *interfaceDescriptor = INTERFACE_DESCRIPTOR_DATA::init();

    uint64_t kernelStartOffset = reinterpret_cast<uint64_t>(allocData.kernelAddr);
    interfaceDescriptor->Bitfield.KernelStartPointerHigh = kernelStartOffset >> 32;
    interfaceDescriptor->Bitfield.KernelStartPointer = (uint32_t)kernelStartOffset >> 0x6;
    interfaceDescriptor->Bitfield.DenormMode = INTERFACE_DESCRIPTOR_DATA::DENORM_MODE_SETBYKERNEL;
    interfaceDescriptor->Bitfield.BindingTablePointer = static_cast<uint32_t>(kernelData->bindingTableState->Offset) >> 0x5;
    interfaceDescriptor.SharedLocalMemorySize = 0u;
    /*
    // threadsPerThreadGroup means number of hardware threads per Work Group
    interfaceDescriptor.NumberOfThreadsInGpgpuThreadGroup = threadsPerThreadGroup;
    */
    // one general purpose register file (GRF) has 32 bytes on GEN9
    uint32_t grfSize = 32;
    size_t crossThreadDataSize = kernelData->dataParameterStream->DataParameterStreamSize;
    interfaceDescriptor->Bitfield.CrossThreadConstantDataReadLength = static_cast<uint32_t>(crossThreadDataSize / grfSize);
    uint32_t numGrfPerThreadData = static_cast<uint32_t>(perThreadDataSize / grfSize);
    numGrfPerThreadData = std::max(numGrfPerThreadData, 1u);
    interfaceDescriptor->Bitfield.ConstantIndirectUrbEntryReadLength = numGrfPerThreadData;
    interfaceDescriptor->Bitfield.BarrierEnable = kernelData->executionEnvironment->HasBarriers;
    return SUCCESS;
}


int Context::createPreemptionAllocation() {
    size_t preemptionSize = 8 * 1048576;
    BufferObject* preemption = allocateBufferObject(preemptionSize);
    if (!preemption) {
        return PREEMPTION_ALLOCATION_FAILED;
    }
    preemption->bufferType = BufferType::PREEMPTION;
    return SUCCESS;
}


int Context::createCommandBuffer() {
    BufferObject* commandBuffer = allocateBufferObject(16 * PAGE_SIZE);
    if (!commandBuffer) {
        return BUFFER_ALLOCATION_FAILED;
    }
    commandBuffer->bufferType = BufferType::COMMAND_BUFFER;

    auto cmd1 = commandBuffer->ptrOffset<MEDIA_STATE_FLUSH*>(sizeof(MEDIA_STATE_FLUSH));
    *cmd1 = MEDIA_STATE_FLUSH::init();

    auto cmd2 = commandBuffer->ptrOffset<MEDIA_INTERFACE_DESCRIPTOR_LOAD*>(sizeof(MEDIA_INTERFACE_DESCRIPTOR_LOAD));
    *cmd2 = MEDIA_INTERFACE_DESCRIPTOR_LOAD::init();
    //cmd2->Bitfield.InterfaceDescriptorDataStartAddress = ;
    cmd2->Bitfield.InterfaceDescriptorTotalLength = sizeof(INTERFACE_DESCRIPTOR_DATA);

    // we need a variable that stores the preemption mode we are using
    // we don't need to dispatch workarounds on Skylake, but maybe on other architectures
    auto cmd3 = commandBuffer->ptrOffset<GPGPU_WALKER*>(sizeof(GPGPU_WALKER));
    *cmd3 = GPGPU_WALKER::init();
    /*
    cmd3->Bitfield.IndirectDataStartAddress = static_cast<uint32_t>(offsetCrossThreadData);
    cmd3->Bitfield.InterfaceDescriptorOffset = interfaceDescriptorIndex;
    cmd3->Bitfield.IndirectDataLength = indirectDataLength;
    cmd3->Bitfield.ThreadWidthCounterMaximum = static_cast<uint32_t>(threadsPerWorkGroup);
    cmd3->Bitfield.ThreadGroupIdXDimension = static_cast<uint32_t>(numWorkGroups[0]);
    cmd3->Bitfield.ThreadGroupIdYDimension = static_cast<uint32_t>(numWorkGroups[1]);
    cmd3->Bitfield.ThreadGroupIdZDimension = static_cast<uint32_t>(numWorkGroups[2]);
    cmd3->Bitfield.RightExecutionMask = static_cast<uint32_t>(executionMask);
    cmd3->Bitfield.BottomExecutionMask = static_cast<uint32_t>(0xffffffff);
    cmd3->Bitfield.SimdSize = simdSize;
    cmd3->Bitfield.ThreadGroupIdStartingX = static_cast<uint32_t>(startWorkGroups[0]);
    cmd3->Bitfield.ThreadGroupIdStartingY = static_cast<uint32_t>(startWorkGroups[1]);
    cmd3->Bitfield.ThreadGroupIdStartingResumeZ = static_cast<uint32_t>(startWorkGroups[2]);
    */

    /*
    auto pCmd4 = reinterpret_cast<MEDIA_STATE_FLUSH*>(pCmd3);
    *pCmd4 = MEDIA_STATE_FLUSH::init();
    pCmd4->Bitfield.InterfaceDescriptorOffset = interfaceDescriptorIndex;
    pCmd4 = pCmd4 + sizeof(MEDIA_STATE_FLUSH);

    // program Cache flush here

    // add pipe control here
    if (isPipeControlWArequired) {  // this is not in WA table, instead manually set to true
        auto pCmd5 = reinterpret_cast<PIPE_CONTROL*>(pCmd4);
        *pCmd5 = PIPE_CONTROL::init();
        pCmd5->Bitfield.CommandStreamerStallEnable = true;
        pCmd5 = pCmd5 + sizeof(PIPE_CONTROL);
    }
    // updateTag, dispatchFlags.guardCommandBufferWithPipeControl (true), 
    // dispatchFlags.memoryMigrationRequired (migratableArgsMap), activePartitions, staticWorkPartitioningEnabled

    // This driver doesn't support command queues with batched submission. So each 
    // kernel submission needs its own pipe control. A batched submission would only 
    // need one pipe control for the whole batch.
    auto pCmd6 = reinterpret_cast<PIPE_CONTROL*>(pCmd5);
    *pCmd6 = PIPE_CONTROL::init();
    pCmd6->Bitfield.CommandStreamerStallEnable = true;
    pCmd6->Bitfield.ConstantCacheInvalidationEnable = false;
    pCmd6->Bitfield.InstructionCacheInvalidateEnable = false;
    pCmd6->Bitfield.PipeControlFlushEnable = false;
    pCmd6->Bitfield.RenderTargetCacheFlushEnable = false;
    pCmd6->Bitfield.StateCacheInvalidationEnable = false;
    pCmd6->Bitfield.TextureCacheInvalidationEnable = false;
    pCmd6->Bitfield.VfCacheInvalidationEnable = false;
    pCmd6->Bitfield.GenericMediaStateClear = false;
    pCmd6->Bitfield.TlbInvalidate = ;

    // has something to do with debugging, is set in DrmCommandStreamReceiver constructor
    // (drm_command_stream.inl)
    pCmd6->Bitfield.NotifyEnable = true;

    // For this driver, DcFlushEnable is always true. If batched submission would be
    // possible with this driver, DcFlushEnable would be true if:
    // - capabilityTable.gpuAddressSpace >= maxNBitValue(47) (maybe true on some 
    //   architectures?)
    // - isFlushL3Required == true (seems to be always the case)
    pCmd6->Bitfield.DcFlushEnable = true;

    pCmd6->Bitfield.PostSyncOperation = PIPE_CONTROL::POST_SYNC_OPERATION_WRITE_IMMEDIATE_DATA;
    pCmd6->Bitfield.Address = static_cast<uint32_t>(tagBufferGpuAddress & 0x0000FFFFFFFFULL);
    pCmd6->Bitfield.AddressHigh = static_cast<uint32_t>(tagBufferGpuAddress >> 32);

    // immediateData == taskCount (always 1 if no batched submission)
    pCmd6->Bitfield.ImmediateData = 1u;
    pCmd6 = pCmd6 + sizeof(PIPE_CONTROL);

    // program Pipeline Select
    if (mediaSamplerConfigChanged || !isPreambleSent) {
        auto pCmd7 = reinterpret_cast<PIPELINE_SELECT*>(pCmd6);
        *pCmd7 = PIPELINE_SELECT::init();
        pCmd7->Bitfield.MaskBits = mask;
        pCmd7->Bitfield.PipelineSelection = PIPELINE_SELECTION_GPGPU;
        pCmd7->Bitfield.MediaSamplerDopClockGateEnable = !pipelineSelectArgs.mediaSamplerRequired;
        pCmd7 = pCmd7 + sizeof(PIPELINE_SELECT);
    }

    // program L3 cache:
    auto pCmd8 = reinterpret_cast<MI_LOAD_REGISTER_IMM*>(pCmd7);
    *pCmd8 = MI_LOAD_REGISTER_IMM::init();
    pCmd8->Bitfield.RegisterOffset = L3RegisterOffset;
    pCmd8->Bitfield.DataDword = l3Config;
    pCmd8 = pCmd8 + sizeof(MI_LOAD_REGISTER_IMM);

    // program Thread Arbitration
    auto pCmd9 = reinterpret_cast<PIPE_CONTROL*>(pCmd8);
    *pCmd9 = PIPE_CONTROL::init();
    pCmd9->Bitfield.CommandStreamerStallEnable = true;
    pCmd9 = pCmd9 + sizeof(PIPE_CONTROL);

    auto pCmd10 = reinterpret_cast<MI_LOAD_REGISTER_IMM*>(pCmd9);
    *pCmd10 = MI_LOAD_REGISTER_IMM::init();
    pCmd10->Bitfield.RegisterOffset = debugControlReg2Address;
    pCmd10->Bitfield.DataDword = requiredThreadArbitrationPolicy;
    pCmd10 = pCmd10 + sizeof(MI_LOAD_REGISTER_IMM);

    // program Preemption
    if (isMidThreadPreemption) {
        auto pCmd11 = reinterpret_cast<GPGPU_CSR_BASE_ADDRESS*>(pCmd10);
        *pCmd11 = GPGPU_CSR_BASE_ADDRESS::init();
        pCmd11->Bitfield.GpgpuCsrBaseAddress = preemptionBufferGpuAddress;
        pCmd11 = pCmd11 + sizeof(GPGPU_CSR_BASE_ADDRESS);
    }

    // program VFE state
    if (mediaVfeStateDirty) {
        auto pCmd12 = reinterpret_cast<MEDIA_VFE_STATE*>(pCmd12);
        *pCmd12 = MEDIA_VFE_STATE::init();
        pCmd12->Bitfield.MaximumNumberOfThreads = maxFrontEndThreads;
        pCmd12->Bitfield.NumberOfUrbEntries = 1;
        pCmd12->Bitfield.UrbEntryAllocationSize = UrbEntryAllocationSize;
        pCmd12->Bitfield.PerThreadScratchSpace = ScratchSizeValueToProgramMediaVfeState;
        pCmd12->Bitfield.StackSize = ScratchSizeValueToProgramMediaVfeState;
        pCmd12->Bitfield.ScratchSpaceBasePointer = lowAddress;
        pCmd12->Bitfield.ScratchSpaceBasePointerHigh = highAddress;
        pCmd12 = pCmd12 + sizeof(MEDIA_VFE_STATE);
    }

    // program Preemption again?

    // Program State Base Address
    auto pCmd13 = reinterpret_cast<PIPE_CONTROL*>(pCmd12);
    *pCmd13 = PIPE_CONTROL::init();
    pCmd13->Bitfield.CommandStreamerStallEnable = true;
    pCmd13->Bitfield.ConstantCacheInvalidationEnable = false;
    pCmd13->Bitfield.InstructionCacheInvalidateEnable = false;
    pCmd13->Bitfield.PipeControlFlushEnable = false;
    pCmd13->Bitfield.RenderTargetCacheFlushEnable = false;
    pCmd13->Bitfield.StateCacheInvalidationEnable = false;
    pCmd13->Bitfield.TextureCacheInvalidationEnable = false;
    pCmd13->Bitfield.VfCacheInvalidationEnable = false;
    pCmd13->Bitfield.GenericMediaStateClear = false;
    pCmd13->Bitfield.TlbInvalidate = false;
    pCmd13->Bitfield.NotifyEnable = false;
    pCmd13->Bitfield.DcFlushEnable = true;
    pCmd13 = pCmd13 + sizeof(PIPE_CONTROL);

    // Additional Pipe Control

    // Add BATCH_BUFFER_START
    */

    return SUCCESS;
}


Buffer::Buffer(BufferObject* dataBuffer) {
    this->mem = dataBuffer->cpuAddress;
    this->gpuAddress = dataBuffer->gpuAddress;
    this->size = alignUp(dataBuffer->size, 4);
}









