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
#define I915_CONTEXT_PRIVATE_PARAM_BOOST    0x80000000
#define I915_CONTEXT_PARAM_GTT_SIZE         0x3
#define I915_CONTEXT_PARAM_VM               0x9
#define I915_CONTEXT_PARAM_ENGINES          0xa
#define I915_CONTEXT_PARAM_PERSISTENCE      0xb
    uint32_t ctx_id;
    uint32_t size;
    uint64_t param;
    uint64_t value;
};

#define I915_PARAM_CHIPSET_ID           4
#define I915_PARAM_REVISION             32
#define I915_PARAM_HAS_EXEC_SOFTPIN     37
#define I915_PARAM_HAS_SCHEDULER        41
#define  I915_SCHEDULER_CAP_PREEMPTION  (1ul << 2)
struct drm_i915_getparam {
    int32_t param;
    int* value;
}; 


/**
 * struct drm_i915_query - Supply an array of struct drm_i915_query_item for the
 * kernel to fill out.
 *
 * Note that this is generally a two step process for each struct
 * drm_i915_query_item in the array:
 *
 * 1. Call the DRM_IOCTL_I915_QUERY, giving it our array of struct
 *    drm_i915_query_item, with &drm_i915_query_item.length set to zero. The
 *    kernel will then fill in the size, in bytes, which tells userspace how
 *    memory it needs to allocate for the blob(say for an array of properties).
 *
 * 2. Next we call DRM_IOCTL_I915_QUERY again, this time with the
 *    &drm_i915_query_item.data_ptr equal to our newly allocated blob. Note that
 *    the &drm_i915_query_item.length should still be the same as what the
 *    kernel previously set. At this point the kernel can fill in the blob.
 *
 * Note that for some query items it can make sense for userspace to just pass
 * in a buffer/blob equal to or larger than the required size. In this case only
 * a single ioctl call is needed. For some smaller query items this can work
 * quite well.
 *
 */
struct drm_i915_query {
    uint32_t num_items;
    uint32_t flags;
    uint64_t items_ptr;
};
struct drm_i915_query_item {
    uint64_t query_id;
#define DRM_I915_QUERY_TOPOLOGY_INFO        1
#define DRM_I915_QUERY_ENGINE_INFO          2
#define DRM_I915_QUERY_PERF_CONFIG          3
#define DRM_I915_QUERY_MEMORY_REGIONS       4
#define DRM_I915_QUERY_HWCONFIG_TABLE       5
    int32_t length;
    uint32_t flags;
    uint64_t data_ptr;
};
struct drm_i915_query_topology_info {
    uint16_t flags;
    uint16_t max_slices;
    uint16_t max_subslices;
    uint16_t max_eus_per_subslice;
    uint16_t subslice_offset;
    uint16_t subslice_stride;
    uint16_t eu_offset;
    uint16_t eu_stride;
    uint8_t data[];
};

enum drm_i915_gem_engine_class {
    I915_ENGINE_CLASS_RENDER        = 0,
    I915_ENGINE_CLASS_COPY          = 1,
    I915_ENGINE_CLASS_VIDEO         = 2,
    I915_ENGINE_CLASS_VIDEO_ENHANCE = 3,
    
    I915_ENGINE_CLASS_INVALID       = -1
};

struct i915_engine_class_instance {
    uint16_t engine_class; // see enum drm_i915_gem_engine_class
    uint16_t engine_instance;
};

struct drm_i915_engine_info {
    struct i915_engine_class_instance engine;
    uint32_t rsvd0;
    uint64_t flags;
    uint64_t capabilities;
#define I915_CAPABILITY_NOT_SPECIFIED                   0
#define I915_VIDEO_CLASS_CAPABILITY_HEVC                (1 << 0)        // expands to 1
#define I915_VIDEO_AND_ENHANCE_CLASS_CAPABILITY_SFC     (1 << 1)        // expands to 2
    uint64_t rsvd1[4];
};

struct drm_i915_query_engine_info {
    uint32_t num_engines;
    uint32_t rsvd[3];
    struct drm_i915_engine_info engines[];
};

struct drm_i915_gem_vm_control {
    uint64_t extensions;
    uint32_t flags;
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



int openDeviceFile();
bool checkDriverVersion(struct gpuInfo* gpuInfo);
int getParamIoctl(struct gpuInfo* gpuInfo, int param, int* paramValue);
void* queryIoctl(struct gpuInfo* gpuInfo, uint32_t queryId, uint32_t queryItemFlags, int32_t length);
void translateTopologyInfo(struct gpuInfo* gpuInfo, struct drm_i915_query_topology_info* topologyInfo);
int createDrmVirtualMemory(struct gpuInfo* gpuInfo);
int queryGttSize(struct gpuInfo* gpuInfo);
void checkNonPersistentContextsSupport(struct gpuInfo* gpuInfo);
void checkPreemptionSupport(struct gpuInfo* gpuInfo);
int enableTurboBoost(struct gpuInfo* gpuInfo);

int allocUserptr(uintptr_t alloc, size_t size, uint64_t flags);









