#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "driver.h"
#include "gpgpu.h"


int gpInitGPU(struct gpuInfo* gpuInfo) {
    int ret;

    //TODO: Discover all devices
    gpuInfo->fd = openDeviceFile();

    //TODO: Add checks for all ioctls
    if (!checkDriverVersion(gpuInfo)) {
        return WRONG_DRIVER_VERSION;
    }
    // query chipset ID
    getParamIoctl(gpuInfo, I915_PARAM_CHIPSET_ID, &gpuInfo->chipset_id);
    getParamIoctl(gpuInfo, I915_PARAM_REVISION, &gpuInfo->revision_id);
    
    // query topology info
    void* topology = queryIoctl(gpuInfo, DRM_I915_QUERY_TOPOLOGY_INFO, 0u, 0);
    auto data = reinterpret_cast<struct drm_i915_query_topology_info*>(topology);
    if (!data) {
        return QUERY_FAILED;
    }
    //TODO: Add checks for topology info
    gpuInfo->maxSliceCount = data->max_slices;
    gpuInfo->maxSubSliceCount = data->max_subslices;
    gpuInfo->maxEUCount = data->max_eus_per_subslice;
    free(data);

    // Query hardware configuration
    // On newer architectures, the GuC controller has an internal data structure containing
    // hardware information (hardware configuration table, HWConfig). My test machine
    // (Gen9, Skylake) has GuC support, but it is not enabled by default. I did load the GuC
    // firmware with the following steps (it might differ on other architectures and kernels):
    // 1. Edit "/etc/default/grub" and extend GRUB_CMDLINE_LINUX_DEFAULT with the parameter
    //    "i915.enable_guc=2"
    // 2. Run "sudo grub-mkconfig -o /boot/grub/grub.cfg"
    // 3. Run "sudo update-initramfs -u"
    // 4. sudo reboot
    // 5. Check with "sudo cat /sys/kernel/debug/dri/0/gt/uc/guc_info" if GuC firmware is
    //    running
    // During the boot process the driver initialisation code will call the function
    // intel_gt_init_hwconfig(struct intel_gt *gt). Before retrieving the HWConfig table
    // (using a dedicated Host/GuC MMIO interface) it checks if GuC is enabled. Then it makes
    // another check in "bool has_table(struct drm_i915_private *i915)". It only returns true
    // if we have Alder Lake or newer.
    // I added "if (IS_SKYLAKE(i915)) {return true;}" and recompiled the i915 module. But this
    // didn't help. Even though I have GuC enabled, it won't work. It seems that Skylake
    // doesn't dupport HWConfig tables. So I am not including parsing code for HwConfig tables
    // because I cannot test it.
    int32_t length = 0;
    void* deviceBlob = queryIoctl(gpuInfo, DRM_I915_QUERY_HWCONFIG_TABLE, 0u, length);
    gpuInfo->HWConfigTable = deviceBlob;
    if (!deviceBlob) {
        printf("Hardware configuration table could not be retrieved\n");
    }

    // Soft-Pinning supported?
    getParamIoctl(gpuInfo, I915_PARAM_HAS_EXEC_SOFTPIN, &gpuInfo->supportsSoftPin);
    if (!gpuInfo->supportsSoftPin) {
        return NO_SOFTPIN_SUPPORT;
    }

    // Enable Turbo Boost
    ret = enableTurboBoost(gpuInfo);
    if (ret) {
        return NO_TURBO_BOOST;
    }

    // query memory info here

    // query engine info
    gpuInfo->engines = queryIoctl(gpuInfo, DRM_I915_QUERY_ENGINE_INFO, 0u, 0);
    if (!gpuInfo->engines) {
        return QUERY_FAILED;
    }
    

    // create virtual memory address space
    ret = createDrmVirtualMemory(gpuInfo);
    if (ret) {
        return VM_CREATION_FAILED;
    }

    // query GTT (Graphics Translation Table) size
    ret = queryGttSize(gpuInfo);
    if (ret) {
        return QUERY_FAILED;
    }

    //getQueueSliceCount(gpuInfo);
    //checkNonPersistentContextsSupport(gpuInfo);
    //checkPreemptionSupport(gpuInfo);
    
    // Initialize memory manager here

    // Setup command stream receivers
    /*
    std::vector<struct engineInfo> engines = {0, 0, 0};
    engines[0] = {EngineType::ENGINE_RCS, EngineUsage::Regular};
    engines[1] = {EngineType::ENGINE_RCS, EngineUsage::LowPriority};
    engines[2] = {defaultEngine, EngineUsage::Internal};
    if (defaultEngine == EngineType::ENGINE_CCS) {
        engines.push_back({EngineType::ENGINE_CCS, EngineUsage::Regular});
    }
    for (auto &engine : engines) {
        
    }
    */

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

void* queryIoctl(struct gpuInfo* gpuInfo, uint32_t queryId, uint32_t queryItemFlags, int32_t length) {
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
    printf("length: %d\n", queryItem.length);
    length = queryItem.length;
    return data;
}

int createDrmVirtualMemory(struct gpuInfo* gpuInfo) {
    int ret;
    struct drm_i915_gem_vm_control ctl = {0};
    
    ret = ioctl(gpuInfo->fd, DRM_IOCTL_I915_GEM_VM_CREATE, &ctl);
    if (ret == 0) {
        if (ctl.vm_id == 0) {
            return -1;          // 0 is reserved for invalid/unassigned ppGTT
        }
        gpuInfo->drmVmId = ctl.vm_id;
    }
    return ret;
}

int enableTurboBoost(struct gpuInfo* gpuInfo) {
    // Even though my test machine (Skylake) has Turbo Boost 2.0, this does not work. 
    // Do we need to specify a context id first?
    // use ftrace to see why we get EINVAL error
    int ret;
    struct drm_i915_gem_context_param contextParam = {};

    contextParam.param = I915_CONTEXT_PRIVATE_PARAM_BOOST;
    contextParam.value = 1;
    ret = ioctl(gpuInfo->fd, DRM_IOCTL_I915_GEM_CONTEXT_SETPARAM, &contextParam);
    return ret;
}

int queryGttSize(struct gpuInfo* gpuInfo) {
    int ret;
    struct drm_i915_gem_context_param contextParam = {};
    contextParam.ctx_id = 0;
    contextParam.param = I915_CONTEXT_PARAM_GTT_SIZE;
    contextParam.value = 0;
    ret = ioctl(gpuInfo->fd, DRM_IOCTL_I915_GEM_CONTEXT_GETPARAM, &contextParam);
    if (ret == 0) {
        gpuInfo->gttSize = contextParam.value;
    }
    return ret;
}

/*
int getMaxGpuFrequency(struct gpuInfo* gpuInfo) {
    int ret;
    std::string path = "/sys/bus/pci/devices/" + pciPath + "/drm" + "/card";

}

uint32_t createDrmContext(struct gpuInfo* gpuInfo, uint32_t drmVmId, bool isSpecialContextRequested) {
    struct drm_i915_gem_context_create_ext gcc = {};
    int ret = ioctl(gpuInfo->fd, DRM_IOCTL_I915_GEM_CONTEXT_CREATE_EXT, &gcc);
    if (ret) {
        return ret;
    }
    if (drmVmId > 0) {
        struct drm_i915_gem_context_param param = {};
        param.ctx_id = gcc.ctx_id;
        param.value = drmVmId;
        param.param = I915_CONTEXT_PARAM_VM;
        ret = ioctl(gpuInfo->fd, DRM_IOCTL_I915_GEM_CONTEXT_SETPARAM, &param);
        if (ret) {
            return ret;
        }
    }
    return gcc.ctx_id;
}
*/




