#include "driver.h"
#include "gpuinfo.h"

void initGPU(struct gpuInfo* gpuInfo) {

    gpuInfo->fileDescriptor = openDeviceFile();
    //TODO: Add checks to validate driver version
    checkDriverVersion();
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

bool checkDriverVersion(int fd, struct gpuInfo* gpuInfo) {
    int ret;
    struct drm_version version = {};

    char name[5] = {};
    version.name = name;
    version.name_len = 5;
    ret = ioctl(fd, DRM_IOCTL_VERSION, &version);
    if (ret) {
        return false;
    }
    name[4] = '\0';
    //TODO: add driver info to gpuInfo struct
    return strcmp(name, "i915") == 0;
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
