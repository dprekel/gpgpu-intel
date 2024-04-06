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

int Context::allocUserptr(int fd, uintptr_t alloc, size_t size, uint32_t flags) {
    int ret;
    drm_i915_gem_userptr userptr = {};  

    userptr.user_ptr = alloc;
    userptr.user_size = size;
    userptr.flags = flags;

    ret = ioctl(fd, DRM_IOCTL_I915_GEM_USERPTR, &userptr);
    if (ret) {
        return -1;
    }
    return 0;    
}

void* CreateBuffer(GPU* gpuInfo, size_t size) {
    void* alloc;
    int ret;

    Context* context = static_cast<Context*>(gpuInfo->context);
    alloc = context->alignedMalloc(size);
    if (!alloc) {
        printf("Malloc not successful!\n");
        return nullptr;
    }

    ret = context->allocUserptr(gpuInfo->fd, reinterpret_cast<uintptr_t>(alloc), size, 0);
    if (ret) {
        printf("Userptr not successful!\n");
        return nullptr;
    }
    // aligned free is needed here
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
    return 0;
}

int CreateContext(GPU* gpuInfo) {
    int ret;
    Context* context = new Context(gpuInfo);
    gpuInfo->context = static_cast<void*>(context);
    ret = context->createDrmContext();
    if (ret) {
        return CONTEXT_CREATION_FAILED;
    }
    return 0;
}














