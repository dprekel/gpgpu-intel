#include "driver.h"
#include "gpuinfo.h"

void initGPU(struct gpuInfo* gpuInfo) {

    gpuInfo->fileDescriptor = openDeviceFile();
    //TODO: Add checks for all ioctls
    if (checkDriverVersion(gpuInfo)) {
        continue;
    }
    // query chipset ID
    getParamIoctl(gpuInfo, I915_PARAM_CHIPSET_ID, &gpuInfo->chipset_id);
    getParamIoctl(gpuInfo, I915_PARAM_REVISION, &gpuInfo->revision_id);
    
}


int openDeviceFile() {
    const char* fileString;
    int fileDescriptor;

    //TODO: add code that reads out the string from the system
    fileString = "/dev/dri/by-path/pci-0000:00:02.0-render";
    fileDescriptor = open(fileString, O_RDWR);
    //TODO: check fileDescriptor
    return fileDescriptor;
}

bool checkDriverVersion(struct gpuInfo* gpuInfo) {
    int ret;
    struct drm_version version = {};

    char name[5] = {};
    version.name = name;
    version.name_len = 5;
    ret = ioctl(gpuInfo->fileDescriptor, DRM_IOCTL_VERSION, &version);
    if (ret) {
        return false;
    }
    name[4] = '\0';
    //TODO: add driver info to gpuInfo struct
    gpuInfo->driver_name = name;
    return strcmp(name, "i915") == 0;
}

int getParamIoctl(struct gpuInfo* gpuInfo, int param, int* paramValue) {
    int ret;
    struct drm_i915_getparam_t getParam = {};
    getParam.param = param;
    getParam.value = paramValue;

    ret = ioctl(gpuInfo->fileDescriptor, DRM_IOCTL_I915_GETPARAM, &getParam);
    return ret;
}


void* allocateAndPinBuffer(size_t size) {
    void* alloc;
    int ret;

    alloc = alignedMalloc(size);
    if (!alloc) {
        return NULL;
    }

    ret = allocUserptr(reinterpret_cast<uintptr_t>(alloc), size, 0);

}



int allocUserptr(uintptr_t alloc, size_t size, uint32_t flags) {
    int ret;
    struct drm_i915_gem_userptr userptr = {};  

    userptr.user_ptr = alloc;
    userptr.user_size = size;
    userptr.flags = flags;

    // can't continue here because I don't have a valid file descriptor. I could read it out from the deviceStruct, but this is too much work. It's better to replace clGetPlatformIDs first.
    //ret = ioctl(
}
