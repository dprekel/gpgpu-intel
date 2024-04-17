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

void* CreateBuffer(GPU* gpuInfo, size_t size) {
    Context* context = static_cast<Context*>(gpuInfo->context);

    BufferObject* dataBuffer = context->allocateBufferObject(size, 0);
    if (!dataBuffer) {
        return nullptr;
    }
    dataBuffer->bufferType = BufferType::BUFFER_HOST_MEMORY;
    
    //context->emitPinningRequest(bo);

    return dataBuffer->alloc;
}

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

int CreateContext(GPU* gpuInfo) {
    int ret;
    Context* context = new Context(gpuInfo);
    gpuInfo->context = static_cast<void*>(context);
    ret = context->createDrmContext();
    if (gpuInfo->nonPersistentContextsSupported) {
        context->setNonPersistentContext();
    }
    if (ret) {
        return CONTEXT_CREATION_FAILED;
    }
    return SUCCESS;
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
        return -1;
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

    //GPGPU_WALKER walkerCmd = {0};
    //walkerCmd
    return 0;
}

int EnqueueNDRangeKernel(GPU* gpuInfo) {
    int ret;

    Context* context = static_cast<Context*>(gpuInfo->context);
    //ret = context->createPreemptionAllocation();
    //ret = context->createIndirectObjectHeap();
    //ret = context->createDynamicStateHeap();
    //ret = context->createSurfaceStateHeap();
    ret = context->createCommandBuffer();
    return SUCCESS;
}











