#include <stdint.h>
#include <asm/ioctl.h>

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

struct drm_i915_getparam {
    int32_t param;
    int* value;
}; 

struct drm_i915_gem_execbuffer2 {
    uint64_t buffers_ptr;
    uint32_t buffer_count;
    uint32_t batch_start_offset;
    uint32_t batch_len;
    uint32_t DR1;
    uint32_t DR4;
    uint32_t num_cliprects;
    uint64_t cliprects_ptr;
    uint64_t flags;
    uint64_t rsvd1;
    uint64_t rsvd2;
};

struct drm_i915_gem_exec_object2 {
    uint32_t handle;
    uint32_t relocation_count;
    uint64_t relocs_ptr;
    uint64_t alignment;
    uint64_t offset;
    uint64_t flags;
    union {
        uint64_t rsvd1;
        uint64_t pad_to_size;
    };
    uint64_t rsvd2;
};

struct drm_i915_gem_context_create_ext {
    uint32_t ctx_id;
    uint32_t flags;
    uint64_t extensions;
};

struct drm_i915_reg_read {
    uint64_t offset;
    uint64_t val;
};

struct drm_i915_gem_userptr {
    uint64_t user_ptr;
    uint64_t user_size;
    uint32_t flags;
    uint32_t handle;
};

struct drm_i915_gem_context_param {
    uint32_t ctx_id;
    uint32_t size;
    uint64_t param;
    uint64_t value;
};

struct drm_i915_query {
    uint32_t num_items;
    uint32_t flags;
    uint64_t items_ptr;
};

struct drm_i915_gem_vm_control {
    uint64_t extensions;
    uint64_t flags;
    uint32_t vm_id;
};

#define DRM_COMMAND_BASE                        0x40
#define DRM_IOCTL_BASE                          'd'
#define DRM_IO(nr)                              _IO(DRM_IOCTL_BASE,nr)
#define DRM_IOR(nr,type)                        _IOR(DRM_IOCTL_BASE,nr,type)
#define DRM_IOW(nr,type)                        _IOW(DRM_IOCTL_BASE,nr,type)
#define DRM_IOWR(nr,type)                       _IOWR(DRM_IOCTL_BASE,nr,type)

#define DRM_IOCTL_VERSION                       DRM_IOWR(0x00, struct drm_version)
#define DRM_IOCTL_I915_GETPARAM                 DRM_IOWR(DRM_COMMAND_BASE + 0x06, struct drm_i915_getparam)
#define DRM_IOCTL_I915_GEM_EXECBUFFER2          DRM_IOW(DRM_COMMAND_BASE + 0x29, struct drm_i915_gem_execbuffer2)
#define DRM_IOCTL_I915_GEM_CONTEXT_CREATE_EXT   DRM_IOWR(DRM_COMMAND_BASE + 0x2d, struct drm_i915_gem_context_create_ext)
#define DRM_IOCTL_I915_REG_READ                 DRM_IOWR(DRM_COMMAND_BASE + 0x31, struct drm_i915_reg_read)
#define DRM_IOCTL_I915_GEM_USERPTR              DRM_IOWR(DRM_COMMAND_BASE + 0x33, struct drm_i915_gem_userptr)
#define DRM_IOCTL_I915_GEM_CONTEXT_GETPARAM     DRM_IOWR(DRM_COMMAND_BASE + 0x34, struct drm_i915_gem_context_param)
#define DRM_IOCTL_I915_GEM_CONTEXT_SETPARAM     DRM_IOWR(DRM_COMMAND_BASE + 0x35, struct drm_i915_gem_context_param)
#define DRM_IOCTL_I915_QUERY                    DRM_IOWR(DRM_COMMAND_BASE + 0x39, struct drm_i915_query)
#define DRM_IOCTL_I915_GEM_VM_CREATE            DRM_IOWR(DRM_COMMAND_BASE + 0x3a, struct drm_i915_gem_vm_control)
#define DRM_IOCTL_I915_GEM_VM_DESTROY           DRM_IOW(DRM_COMMAND_BASE + 0x3b, struct drm_i915_gem_vm_control)


void initGPU(struct gpuInfo* gpuInfo);
int openDeviceFile();
bool checkDriverVersion(int fd, struct gpuInfo* gpuInfo);
void* allocateAndPinBuffer(size_t size);
int allocUserptr(uintptr_t alloc, size_t size, uint64_t flags);









