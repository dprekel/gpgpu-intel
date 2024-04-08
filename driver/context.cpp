#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <vector>

#include "gpgpu.h"
#include "context.h"
#include "drm_structs.h"


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
    bo->alloc = alloc;
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

int Context::createIndirectObjectHeap() {
    size_t localWorkSize = 16;            // from clEnqueueNDRangeKernel argument
    uint8_t numChannels = 3;             // from kernelInfo.kernelDescriptor.kernelAttributes.numLocalIdChannels
    uint32_t grfSize = 32;                // from sizeof(typename GfxFamily::GRF)
    uint32_t crossThreadDataSize = 96;    // from kernel
    uint32_t simdSize = 16;               // from kernelInfo.kernelDescriptor.kernelAttributes.simdSize

    uint32_t numGRFsPerThread = (simd == 32 && grfSize == 32) ? 2 : 1;
    uint32_t perThreadSizeLocalIDs = numGRFsPerThread * grfSize * (simd == 1 ? 1u : numChannels);
    perThreadSizeLocalIDs = std::max(perThreadSizeLocalIDs, grfSize);
    uint64_t threadsPerWG = simdSize + localWorkSize - 1;
    threadsPerWG >>= simdSize == 32 ? 5 : simdSize == 16 ? 4 : simdSize == 8 ? 3 : 0;

    // in matmul test example, size will be 192
    uint64_t size = crossThreadDataSize + threadsPerWG * perThreadSizeLocalIDs;
    // here, size must be aligned to cache line size and page size
    size = 4096;
    // allocate userptr
    //

    return SUCCESS;
}

int EnqueueNDRangeKernel(GPU* gpuInfo) {
    int ret;

    Context* context = static_cast<Context*>(gpuInfo->context);
    ret = context->createPreemptionAllocation();
    ret = context->createIndirectObjectHeap();
    return SUCCESS;
}












