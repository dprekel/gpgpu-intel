
#define __user

typedef size_t __kernel_size_t;


struct drm_version {
	int version_major;
	int version_minor;
	int version_patchlevel;
	__kernel_size_t name_len;
	char __user *name;
	__kernel_size_t data_len;
	char __user *date;
	__kernel_size_t desc_len;
	char __user desc;
};

#if defined(__cplusplus)
extern "C" {
#endif

#define DRM_IOCTL_BASE                  'd'
#define DRM_IOWR(nr,type)               _IOWR(DRM_IOCTL_BASE,nr,type)
#define DRM_IOCTL_VERSION               DRM_IOWR(0x00, struct drm_version)

}
