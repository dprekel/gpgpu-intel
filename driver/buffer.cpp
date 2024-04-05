#include "context.h"
#include "drm_structs.h"

void* gpCreateBuffer(GPU* gpuInfo, size_t size) {
    void* alloc;
    int ret;

    alloc = alignedMalloc(size);
    if (!alloc) {
        return nullptr;
    }

    ret = allocUserptr(gpuInfo->fd, reinterpret_cast<uintptr_t>(alloc), size, 0);
    if (ret) {
        return nullptr;
    }
    // aligned free is needed here

}

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
    
}
