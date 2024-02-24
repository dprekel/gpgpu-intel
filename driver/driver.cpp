#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "driver.h"
#include "gpgpu.h"


int gpInitGPU(struct gpuInfo* gpuInfo) {

    gpuInfo->fd = openDeviceFile();
    //TODO: Add checks for all ioctls
    if (!checkDriverVersion(gpuInfo)) {
        return WRONG_DRIVER_VERSION;
    }
    // query chipset ID
    getParamIoctl(gpuInfo, I915_PARAM_CHIPSET_ID, &gpuInfo->chipset_id);
    getParamIoctl(gpuInfo, I915_PARAM_REVISION, &gpuInfo->revision_id);
    
    // query topology info
    void* topology = queryIoctl(gpuInfo, DRM_I915_QUERY_TOPOLOGY_INFO, 0u);
    auto data = reinterpret_cast<struct drm_i915_query_topology_info*>(topology);
    if (!data) {
        return FAILED_TOPOLOGY_QUERY;
    }
    gpuInfo->maxSliceCount = data->max_slices;
    gpuInfo->maxSubSliceCount = data->max_subslices;
    gpuInfo->maxEUCount = data->max_eus_per_subslice;
    printf("maxEUCount: %u\n", gpuInfo->maxEUCount);
    // how do I free topology and data?
    free(data);






    return SUCCESS;
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
    ret = ioctl(gpuInfo->fd, DRM_IOCTL_VERSION, &version);
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
    struct drm_i915_getparam getParam = {};
    getParam.param = param;
    getParam.value = paramValue;

    ret = ioctl(gpuInfo->fd, DRM_IOCTL_I915_GETPARAM, &getParam);
    return ret;
}

void* queryIoctl(struct gpuInfo* gpuInfo, uint32_t queryId, uint32_t queryItemFlags) {
    int ret;
    struct drm_i915_query query = {};
    struct drm_i915_query_item queryItem = {};
    queryItem.query_id = queryId;
    queryItem.length = 0;
    queryItem.flags = queryItemFlags;
    query.items_ptr = reinterpret_cast<uint64_t>(&queryItem);
    query.num_items = 1;

    ret = ioctl(gpuInfo->fd, DRM_IOCTL_I915_QUERY, &query);
    if (ret != 0 || queryItem.length <= 0) {
        return nullptr;
    }

    void* data = malloc(queryItem.length);
    memset(data, 0, queryItem.length);
    queryItem.data_ptr = reinterpret_cast<uint64_t>(data);

    ret = ioctl(gpuInfo->fd, DRM_IOCTL_I915_QUERY, &query);
    if (ret != 0 || queryItem.length <= 0) {
        return nullptr;
    }
    return data;
} 




/*
void* gpAllocateAndPinBuffer(size_t size) {
    void* alloc;
    int ret;

    alloc = alignedMalloc(size);
    if (!alloc) {
        return NULL;
    }

    ret = allocUserptr(reinterpret_cast<uintptr_t>(alloc), size, 0);

}
*/


int allocUserptr(uintptr_t alloc, size_t size, uint32_t flags) {
    int ret;
    struct drm_i915_gem_userptr userptr = {};  

    userptr.user_ptr = alloc;
    userptr.user_size = size;
    userptr.flags = flags;

    // can't continue here because I don't have a valid file descriptor. I could read it out from the deviceStruct, but this is too much work. It's better to replace clGetPlatformIDs first.
    //ret = ioctl(
}
