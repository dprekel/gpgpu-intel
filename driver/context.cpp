#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <vector>

#include "gpgpu.h"
#include "context.h"
#include "drm_structs.h"
#include "avx.h"


Context::Context(GPU* gpuInfo) 
         : gpuInfo(gpuInfo) {
}

Context::~Context() {}

void* Context::alignedMalloc(size_t size) {
    size_t alignment = 4096;  // page size
    size_t sizeToAlloc = size + alignment;
    void* pOriginalMemory = malloc(sizeToAlloc);

    uintptr_t pAlignedMemory = reinterpret_cast<uintptr_t>(pOriginalMemory);
    if (pAlignedMemory) {
        pAlignedMemory += alignment;
        pAlignedMemory -= pAlignedMemory % alignment;
        reinterpret_cast<void**>(pAlignedMemory)[-1] = pOriginalMemory;
    }
    return reinterpret_cast<void*>(pAlignedMemory);
}

BufferObject* Context::allocUserptr(int fd, uintptr_t alloc, size_t size, uint32_t flags) {
    int ret;
    drm_i915_gem_userptr userptr = {};  

    userptr.user_ptr = alloc;
    userptr.user_size = size;
    userptr.flags = flags;

    ret = ioctl(fd, DRM_IOCTL_I915_GEM_USERPTR, &userptr);
    if (ret) {
        return nullptr;
    }
    BufferObject* bo = new BufferObject();
    bo->bufferType = BufferType::BUFFER_HOST_MEMORY;
    bo->alloc = reinterpret_cast<void*>(alloc);
    bo->size = size;
    bo->handle = userptr.handle;
    return bo;
}

void Context::enqueueBufferObject(BufferObject* bo) {
    execBuffer.push_back(bo);
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
    void* alloc;
    int ret;

    Context* context = static_cast<Context*>(gpuInfo->context);
    alloc = context->alignedMalloc(size);
    if (!alloc) {
        printf("Malloc not successful!\n");
        return nullptr;
    }

    BufferObject* bo = context->allocUserptr(gpuInfo->fd, reinterpret_cast<uintptr_t>(alloc), size, 0);
    if (!bo) {
        printf("Userptr not successful!\n");
        return nullptr;
    }
    // aligned free is needed here
    
    bo->bufferType = BufferType::BUFFER_HOST_MEMORY;
    context->enqueueBufferObject(bo);
    //context->emitPinningRequest(bo);

    return alloc;
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

int Context::createPreemptionAllocation() {
    return SUCCESS;
}

void* ptrOffset(void* ptrBefore, size_t offset) {
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
    uint8_t numChannels = 3;             // from kernelInfo.kernelDescriptor.kernelAttributes.numLocalIdChannels
    uint32_t grfSize = 32;                // from sizeof(typename GfxFamily::GRF)
    uint32_t crossThreadDataSize = 96;    // from kernel
    uint32_t simdSize = 16;               // from kernelInfo.kernelDescriptor.kernelAttributes.simdSize

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
    
    //generateLocalIDsSimd(buffer, localWorkgroupSize, threadsPerWorkGroup, dimensionsOrder, simdSize);


    return SUCCESS;
}

int EnqueueNDRangeKernel(GPU* gpuInfo) {
    int ret;

    Context* context = static_cast<Context*>(gpuInfo->context);
    ret = context->createPreemptionAllocation();
    ret = context->createIndirectObjectHeap();
    ret = context->createDynamicStateHeap();
    ret = context->createSurfaceStateHeap();
    return SUCCESS;
}











