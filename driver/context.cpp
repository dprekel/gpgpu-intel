#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <vector>

#include "gpgpu.h"
#include "context.h"
#include "drm_structs.h"
#include "avx.h"
#include "commands_gen9.h"


Context::Context(GPU* gpuInfo) 
         : gpuInfo(gpuInfo) {
}

Context::~Context() {}

BufferObject* Context::allocateBufferObject(size_t size, uint32_t flags) {
    size_t alignment = 4096;    // aligned to page size
    size_t sizeToAlloc = size + alignment;
    void* pOriginalMemory = malloc(sizeToAlloc);

    uintptr_t pAlignedMemory = reinterpret_cast<uintptr_t>(pOriginalMemory);
    if (pAlignedMemory) {
        pAlignedMemory += alignment;
        pAlignedMemory -= pAlignedMemory % alignment;
        reinterpret_cast<void**>(pAlignedMemory)[-1] = pOriginalMemory;
    }
    else {
        return nullptr;
    }
    drm_i915_gem_userptr userptr = {};
    userptr.user_ptr = pAlignedMemory;
    userptr.user_size = size;
    userptr.flags = flags;

    int ret = ioctl(gpuInfo->fd, DRM_IOCTL_I915_GEM_USERPTR, &userptr);
    if (ret) {
        return nullptr;
    }
    // add aligned free here
    BufferObject* bo = new BufferObject();
    bo->alloc = reinterpret_cast<void*>(pAlignedMemory);
    bo->size = size;
    bo->handle = userptr.handle;
    execBuffer.push_back(bo);
    return bo;
}

/*
int Context::emitPinningRequest(BufferObject* bo) {
    drm_i915_gem_exec_object2 execObject = {};
    execObject.handle = bo->handle;
    execObject.relocations_count = 0;
    execObject.relocs_ptr = 0ul;
    execObject.alignment = 0;
    execObject.offset = bo->alloc;
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
    vmId = gpuInfo->drmVmId;
    
    drm_i915_gem_context_create_ext gcc = {};
    ret = ioctl(gpuInfo->fd, DRM_IOCTL_I915_GEM_CONTEXT_CREATE_EXT, &gcc);
    if (ret) {
        return ret;
    }
    if (vmId > 0) {
        drm_i915_gem_context_param param = {};
        param.ctx_id = gcc.ctx_id;
        param.value = vmId;
        param.param = I915_CONTEXT_PARAM_VM;
        ret = ioctl(gpuInfo->fd, DRM_IOCTL_I915_GEM_CONTEXT_SETPARAM, &param);
        if (ret) {
            return ret;
        }
    }
    ctxId = gcc.ctx_id;
    return SUCCESS;
}

void Context::setNonPersistentContext() {
    drm_i915_gem_context_param contextParam = {};
    contextParam.ctx_id = ctxId;
    contextParam.param = I915_CONTEXT_PARAM_PERSISTENCE;
    ioctl(gpuInfo->fd, DRM_IOCTL_I915_GEM_CONTEXT_SETPARAM, &contextParam);
}


/*
int Context::createPreemptionAllocation() {
    return SUCCESS;
}

void* Context::ptrOffset(void* ptrBefore, size_t offset) {
    uintptr_t addrAfter = (uintptr_t)ptrBefore + offset;
    return (void*)addrAfter;
}



void Context::generateLocalIDsSimd(void* b, uint16_t* localWorkgroupSize, uint16_t threadsPerWorkGroup, uint8_t* dimensionsOrder, uint32_t simdSize) {
    const int passes = simdSize / 8;
    int pass = 0;

    __m256i vLwsX = _mm256_set1_epi16(16);   // localWorkgroupSize[xDimNum] == 16
    __m256i vLwsY = _mm256_set1_epi16(1);    // localWorkgroupSize[yDimNum] == 1

    __m256i zero = _mm256_set1_epi16(0u);
    __m256i one = _mm256_set1_epi16(1u);

    uint64_t threadSkipSize;
    if (simdSize == 32) {
        threadSkipSize = 32 * sizeof(uint16_t);
    }
    else {
        threadSkipSize = 16 * sizeof(uint16_t);
    }
    __m256i vSimdX = _mm256_set1_epi16(simdSize);
    __m256i vSimdY = zero;
    __m256i vSimdZ = zero;

    __m256i xWrap;
    __m256i yWrap;

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
    } while (xWrap || yWrap);
    
    do {
        void* buffer = b;

        __m256i x = ;
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
        } while (xWrap);

        for (size_t i = 0; i < threadsPerWorkGroup; ++i) {
            __mm256_store_si256(reinterpret_cast<__mm256i*>(ptrOffset(buffer, xDimNum * threadSkipSize)), x);
            __mm256_store_si256(reinterpret_cast<__mm256i*>(ptrOffset(buffer, yDimNum * threadSkipSize)), y);
            __mm256_store_si256(reinterpret_cast<__mm256i*>(ptrOffset(buffer, zDimNum * threadSkipSize)), z);

            x += vSimdX;
            y += vSimdY;
            z += vSimdZ;
            xWrap = x >= vLwsX;
            __mm256i deltaX = _mm256_blendv_epi8(vLwsX, zero, xWrap);
            x -= deltaX;
            __mm256i deltaY = _mm256_blendv_epi8(one, zero, xWrap);
            y += deltaY;
            yWrap = y >= vLwsY;
            __mm256i deltaY2 = _mm256_blendv_epi8(one, zero, yWrap);
            y -= deltaY2;
            __mm256i deltaZ = _mm256_blendv_epi8(one, zero, yWrap);
            z += deltaZ;
            buffer = ptrOffset(buffer, 3 * threadSkipSize);
        }
        b = ptrOffset(b, 8 * sizeof(uint16_t));
    } while (++pass < passes);

}


int Context::createIndirectObjectHeap() {
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
    // here, size must be aligned to cache line size and page size
    size = 4096;
    // allocate userptr
    
    char* crossThreadData;          // from kernel.kernelInfo.crossThreadData
    // align ioh here
    setCrossThreadData(ioh, crossThreadData);
    generateLocalIDsSimd(ioh, localWorkgroupSize, threadsPerWorkGroup, dimensionsOrder, simdSize);


    return SUCCESS;
}


data needed:- ssh pointer
            - kernel.kernelInfo.kernelDescriptor.payloadMappings.bindingTable.numEntries
            - kernel.pSshLocal
            - kernel.sshLocalSize
            - kernel.numberOfBindingTableStates (from kernel.kernelInfo.kernelDescriptor.payloadMappings.bindingTable.numEntries)
            - kernel.localBindingTableOffset (from kernel.kernelDescriptor.payloadMappings.bindingTable.tableOffset)
- populateKernelDescriptor() in kernel_descriptor_from_patchtokens.cpp copies data from src.tokens to kernel.kernelDescriptor
- IMPORTANT: file patchtokens_decoder.cpp

int Context::createSurfaceStateHeap() {
    BINDING_TABLE_STATE bti = {0};

}

int Context::createDynamicStateHeap() {
    INTERFACE_DESCRIPTOR_DATA interfaceDescriptor = {0};
    interfaceDescriptor.KernelStartPointerHigh = kernelStartOffset >> 32;
    interfaceDescriptor.KernelStartPointer = (uint32_t)kernelStartOffset >> 0x6;
    interfaceDescriptor.DenormMode = 0x1;
    interfaceDescriptor.SamplerStatePointer = static_cast<uint32_t>(offsetSamplerState) >> 0x5;
    interfaceDescriptor.BindingTablePointer = static_cast<uint32_t>(bindingTablePointer) >> 0x5;
    interfaceDescriptor.SharedLocalMemorySize = programmableIDSLMSize;
    interfaceDescriptor.NumberOfThreadsInGpgpuThreadGroup = threadsPerThreadGroup;
    interfaceDescriptor.CrossThreadConstantDataReadLength = numGrfCrossThreadData;
    interfaceDescriptor.ConstantIndirectUrbEntryReadLength = numGrfPerThreadData;
    interfaceDescriptor.BarrierEnable = barrierCount;       // from kernel.kernelInfo.kernelDescriptor.kernelAttributes.barrierCount
}
*/

int Context::createCommandBuffer() {
    BufferObject* commandBuffer = allocateBufferObject(65536, 0);
    if (!commandBuffer) {
        return BUFFER_ALLOCATION_FAILED;
    }
    commandBuffer->bufferType = BufferType::COMMAND_BUFFER;

    auto pCmd1 = reinterpret_cast<MEDIA_STATE_FLUSH*>(commandBuffer->alloc);
    *pCmd1 = MEDIA_STATE_FLUSH::init();
    pCmd1 = pCmd1 + sizeof(MEDIA_STATE_FLUSH);
    //printf("MEDIA_STATE_FLUSH: %lu\n", sizeof(MEDIA_STATE_FLUSH));

    auto pCmd2 = reinterpret_cast<MEDIA_INTERFACE_DESCRIPTOR_LOAD*>(pCmd1);
    *pCmd2 = MEDIA_INTERFACE_DESCRIPTOR_LOAD::init();
    //*pCmd.Bitfield.InterfaceDescriptorDataStartAddress = ;
    pCmd2->Bitfield.InterfaceDescriptorTotalLength = sizeof(INTERFACE_DESCRIPTOR_DATA);
    pCmd2 = pCmd2 + sizeof(MEDIA_INTERFACE_DESCRIPTOR_LOAD);

    // we need a variable that stores the preemption mode we are using
    // we don't need to dispatch workarounds on Skylake, but maybe on other architectures
    auto pCmd3 = reinterpret_cast<GPGPU_WALKER*>(pCmd2);
    *pCmd3 = GPGPU_WALKER::init();
    pCmd3->Bitfield.InterfaceDescriptorOffset = interfaceDescriptorIndex;
    pCmd3->Bitfield.IndirectDataLength = indirectDataLength;
    pCmd3->Bitfield.ThreadWidthCounterMaximum = static_cast<uint32_t>(threadsPerWorkGroup);
    pCmd3->Bitfield.ThreadGroupIdXDimension = static_cast<uint32_t>(numWorkGroups[0]);
    pCmd3->Bitfield.ThreadGroupIdYDimension = static_cast<uint32_t>(numWorkGroups[1]);
    pCmd3->Bitfield.ThreadGroupIdZDimension = static_cast<uint32_t>(numWorkGroups[2]);
    pCmd3->Bitfield.RightExecutionMask = static_cast<uint32_t>(executionMask);
    pCmd3->Bitfield.BottomExecutionMask = static_cast<uint32_t>(0xffffffff);
    pCmd3->Bitfield.SimdSize = simdSize;
    pCmd3->Bitfield.ThreadGroupIdStartingX = static_cast<uint32_t>(startWorkGroups[0]);
    pCmd3->Bitfield.ThreadGroupIdStartingY = static_cast<uint32_t>(startWorkGroups[1]);
    pCmd3->Bitfield.ThreadGroupIdStartingResumeZ = static_cast<uint32_t>(startWorkGroups[2]);
    pCmd3 = pCmd3 + sizeof(GPGPU_WALKER);

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
    auto pCmd6 = reinterpret_cast<PIPE_CONTROL*>(pCmd5);
    *pCmd6 = PIPE_CONTROL::init();
    pCmd6->Bitfield.CommandStreamerStallEnable = true;
    pCmd6->Bitfield.ConstantCacheInvalidationEnable(args.constantCacheInvalidationEnable);
    pCmd6->Bitfield.InstructionCacheInvalidateEnable(args.instructionCacheInvalidateEnable);
    pCmd6->Bitfield.PipeControlFlushEnable(args.pipeControlFlushEnable);
    pCmd6->Bitfield.RenderTargetCacheFlushEnable(args.renderTargetCacheFlushEnable);
    pCmd6->Bitfield.StateCacheInvalidationEnable(args.stateCacheInvalidationEnable);
    pCmd6->Bitfield.TextureCacheInvalidationEnable(args.textureCacheInvalidationEnable);
    pCmd6->Bitfield.VfCacheInvalidationEnable(args.vfCacheInvalidationEnable);
    pCmd6->Bitfield.GenericMediaStateClear(args.genericMediaStateClear);
    pCmd6->Bitfield.TlbInvalidate(args.tlbInvalidation);
    pCmd6->Bitfield.setNotifyEnable(args.notifyEnable);
    if (isDcFlushAllowed) {
        pCmd6->Bitfield.DcFlushEnable(args.dcFlushEnable);
    }
    pCmd6->Bitfield.PostSyncOperation = operation;
    pCmd6->Bitfield.Address = static_cast<uint32_t>(tagBufferGpuAddress & 0x0000FFFFFFFFULL);
    pCmd6->Bitfield.AddressHigh = static_cast<uint32_t>(tagBufferGpuAddress >> 32);
    if (operation == POST_SYNC_OPERATION_WRITE_IMMEDIATE_DATA) {
        pCmd6->Bitfield.ImmediateData = immediateData;
    }
    pCmd6 = pCmd6 + sizeof(PIPE_CONTROL);

    // program Pipeline Select
    if (mediaSamplerConfigChanged || !isPreambleSent) {
        auto pCmd7 = reinterpret_cast<PIPELINE_SELECT*>(pCmd6);;
        *pCmd7 = PIPELINE_SELECT::init();
        pCmd7->Bitfield.MaskBits = mask;
        pCmd7->Bitfield.PipelineSelection = PIPELINE_SELECTION_GPGPU;
        pCmd7->Bitfield.MediaSamplerDopClockGateEnable = !pipelineSelectArgs.mediaSamplerRequired;
        pCmd7 = pCmd7 + sizeof(PIPELINE_SELECT);
    }

    return 0;
}












