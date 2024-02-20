#define DRM_I915_GETPARAM                   0x06
#define DRM_I915_REG_READ                   0x31
#define DRM_I915_GEM_USERPTR                0x33
#define DRM_I915_GEM_CONTEXT_GETPARAM       0x34
#define DRM_I915_GEM_CONTEXT_SETPARAM       0x34
#define DRM_I915_GEM_VM_CREATE              0x3a
#define DRM_I915_GEM_VM_DESTROY             0x3b
#define DRM_I915_QUERY
#define DRM_I915_GEM_CONTEXT_CREATE_EXT


#define __user
struct drm_version {
    int version_major;
    int version_minor;
    int version_patchlevel;
    size_t name_len;
    char __user* name;
    size_t data_len;
    char __user* date;
    size_t desc_len;
    char __user* desc;
};

struct drm_i915_gem_userptr {
    uint64_t user_ptr;
    uint64_t user_size;
    uint32_t flags;
    uint32_t handle;
};

void initGPU(struct gpuInfo* gpuInfo);
int openDeviceFile();
bool checkDriverVersion(int fd, struct gpuInfo* gpuInfo);
void* allocateAndPinBuffer(size_t size);
int allocUserptr(uintptr_t alloc, size_t size, uint64_t flags);
