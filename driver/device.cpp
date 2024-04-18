#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "device.h"
#include "hwinfo.h"
#include "skl_info.h"
#include "gpgpu.h"
#include "drm_structs.h"

const DeviceDescriptor deviceDescriptorTable[] = {
#define NAMEDDEVICE(devId, gt, gtType, devName) {devId, &gt::hwInfo, &gt::setupHardwareInfo, gtType, devName},
#define DEVICE(devId, gt, gtType) {devId, &gt::hwInfo, &gt::setupHardwareInfo, gtType, ""},
#include "devices_base.h"
#undef DEVICE
#undef NAMEDDEVICE
    {0, nullptr, nullptr, GTTYPE::GTTYPE_UNDEFINED}
};

Device::Device(GPU* gpuInfo) 
         : gpuInfo(gpuInfo) {
}

Device::~Device() {}

int Device::initializeGPU() {
    int ret;

    //TODO: Discover all devices
    gpuInfo->fd = openDeviceFile();

    //TODO: Add checks for all ioctls
    if (!checkDriverVersion()) {
        return WRONG_DRIVER_VERSION;
    }
    // query chipset ID
    getParamIoctl(I915_PARAM_CHIPSET_ID, &gpuInfo->chipset_id);
    getParamIoctl(I915_PARAM_REVISION, &gpuInfo->revision_id);
    
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
    void* deviceBlob = queryIoctl(DRM_I915_QUERY_HWCONFIG_TABLE, 0u, length);
    gpuInfo->HWConfigTable = deviceBlob;
    if (!deviceBlob) {
        printf("Hardware configuration table could not be retrieved\n");
    }
    // setup System info from device blob
    DeviceDescriptor* descriptor = new DeviceDescriptor();
    gpuInfo->descriptor = static_cast<void*>(descriptor);
    for (auto &d : deviceDescriptorTable) {
        if (gpuInfo->chipset_id == d.deviceId) {
            descriptor->pHwInfo = d.pHwInfo;
            descriptor->setupHardwareInfo = d.setupHardwareInfo;
            //gpuInfo->eGtType = d.eGtType;
            descriptor->devName = d.devName;
            break;
        }
    }
    if (descriptor->pHwInfo) {
        descriptor->setupHardwareInfo(descriptor->pHwInfo);
        descriptor->pHwInfo->platform->usDeviceID = gpuInfo->chipset_id;
        descriptor->pHwInfo->platform->usRevId = gpuInfo->revision_id;
    }

    // query topology info
    void* topology = queryIoctl(DRM_I915_QUERY_TOPOLOGY_INFO, 0u, 0);
    auto data = reinterpret_cast<struct drm_i915_query_topology_info*>(topology);
    if (!data) {
        return QUERY_FAILED;
    }

    //TODO: Add checks for topology info
    translateTopologyInfo(data);
    free(data);

    // Soft-Pinning supported?
    getParamIoctl(I915_PARAM_HAS_EXEC_SOFTPIN, &gpuInfo->supportsSoftPin);
    if (!gpuInfo->supportsSoftPin) {
        return NO_SOFTPIN_SUPPORT;
    }

    // Enable Turbo Boost
    ret = enableTurboBoost();
    /*
    if (ret) {
        return NO_TURBO_BOOST;
    }
    */

    // query memory info here

    // query engine info
    gpuInfo->engines = queryIoctl(DRM_I915_QUERY_ENGINE_INFO, 0u, 0);
    if (!gpuInfo->engines) {
        return QUERY_FAILED;
    }
    
    // create virtual memory address space
    ret = createDrmVirtualMemory();
    if (ret) {
        return VM_CREATION_FAILED;
    }

    // query GTT (Graphics Translation Table) size
    ret = queryGttSize();
    if (ret) {
        return QUERY_FAILED;
    }

    // ask all engines if slice count change is supported
    //getQueueSliceCount(gpuInfo);

    // check if non-persistent contexts are supported. A non-persistent context gets
    // destroyed immediately upon closure (through DRM_I915_GEM_CONTEXT_CLOSE, fd destruction 
    // or process termination). A persistent context can finish the batch before closing.
    checkNonPersistentContextsSupport();

    checkPreemptionSupport();
    
    // Initialize memory manager here

    // Set up command stream receivers
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


int Device::openDeviceFile() {
    const char* fileString;
    int fileDescriptor;

    //TODO: add code that reads out the string from the system
    fileString = "/dev/dri/by-path/pci-0000:00:02.0-render";
    fileDescriptor = open(fileString, O_RDWR);
    //TODO: check fileDescriptor
    return fileDescriptor;
}

bool Device::checkDriverVersion() {
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

int Device::getParamIoctl(int param, int* paramValue) {
    int ret;
    struct drm_i915_getparam getParam = {};
    getParam.param = param;
    getParam.value = paramValue;

    ret = ioctl(gpuInfo->fd, DRM_IOCTL_I915_GETPARAM, &getParam);
    return ret;
}

void* Device::queryIoctl(uint32_t queryId, uint32_t queryItemFlags, int32_t length) {
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

void Device::translateTopologyInfo(struct drm_i915_query_topology_info* topologyInfo) {
    uint16_t sliceCount = 0;
    uint16_t subSliceCount = 0;
    uint16_t euCount = 0;
    uint16_t subSliceCountPerSlice = 0;
    uint16_t euCountPerSubSlice = 0;
    for (int x = 0; x < topologyInfo->max_slices; x++) {
        bool isSliceEnabled = (topologyInfo->data[x / 8] >> (x % 8)) & 1;
        if (!isSliceEnabled) {
            continue;
        }
        sliceCount++;
        for (int y = 0; y < topologyInfo->max_subslices; y++) {
            size_t yOffset = (topologyInfo->subslice_offset + x * topologyInfo->subslice_stride + y / 8);
            bool isSubSliceEnabled = (topologyInfo->data[yOffset] >> (y % 8)) & 1;
            if (!isSubSliceEnabled) {
                continue;
            }
            subSliceCount++;
            if ((x == 0) && (subSliceCount > y)) {
                subSliceCountPerSlice = subSliceCount;
            }
            for (int z = 0; z < topologyInfo->max_eus_per_subslice; z++) {
                size_t zOffset = (topologyInfo->eu_offset + (x * topologyInfo->max_subslices + y) * topologyInfo->eu_stride + z / 8);
                bool isEUEnabled = (topologyInfo->data[zOffset] >> (z % 8)) & 1;
                if (!isEUEnabled) {
                    continue;
                }
                euCount++;
                if ((x == 0) && (y == 0) && (euCount > z)) {
                    euCountPerSubSlice = euCount;
                }
            }
        }
    }
    gpuInfo->sliceCount = sliceCount;
    gpuInfo->subSliceCount = subSliceCount;
    gpuInfo->euCount = euCount;
    gpuInfo->subSliceCountPerSlice = subSliceCountPerSlice;
    gpuInfo->euCountPerSubSlice = euCountPerSubSlice;
}

int Device::createDrmVirtualMemory() {
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

int Device::enableTurboBoost() {
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

int Device::queryGttSize() {
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

void Device::checkNonPersistentContextsSupport() {
    int ret;
    struct drm_i915_gem_context_param contextParam = {};
    contextParam.param = I915_CONTEXT_PARAM_PERSISTENCE;

    ret = ioctl(gpuInfo->fd, DRM_IOCTL_I915_GEM_CONTEXT_GETPARAM, &contextParam);
    if (ret == 0 && contextParam.value == 1) {
        gpuInfo->nonPersistentContextsSupported = true;
    }
    else {
        gpuInfo->nonPersistentContextsSupported = false;
    }
}

void Device::checkPreemptionSupport() {
    int ret;
    int value = 0;
    ret = getParamIoctl(I915_PARAM_HAS_SCHEDULER, &value);
    gpuInfo->schedulerValue = value;
    gpuInfo->preemptionSupported = ((0 == ret) && (value & I915_SCHEDULER_CAP_PREEMPTION));
}

/*
int Device::getMaxGpuFrequency(GPU* gpuInfo) {
    int ret;
    std::string path = "/sys/bus/pci/devices/" + pciPath + "/drm" + "/card";

}
*/





