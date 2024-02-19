struct drm_i915_gem_userptr {
    uint64_t user_ptr;
    uint64_t user_size;
    uint32_t flags;
    uint32_t handle;
};

void* allocateAndPinBuffer(size_t size);
int allocUserptr(uintptr_t alloc, size_t size, uint64_t flags);
