#include <dirent.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include <memory>
#include <string>

#include "device.h"
#include "gpgpu.h"
#include "hwinfo.h"
#include "ioctl.h"

const DeviceDescriptor deviceDescriptorTable[] = {
#define NAMEDDEVICE(devId, gt, devName) {devId, &gt::hwInfo, &gt::setupHardwareInfo, devName},
#define DEVICE(devId, gt) {devId, &gt::hwInfo, &gt::setupHardwareInfo, ""},
#include "devices_base.h"
#undef DEVICE
#undef NAMEDDEVICE
    {0, nullptr, nullptr}
};

Device::Device(int fd) : fd(fd)
                         {}

Device::~Device() {
    printf("Device destructor called!\n");
}

int Device::initialize() {
    numDevices += 1;
    if (numDevices == 1) {
        bool isi915 = checkDriverVersion();
        if (isi915 == false)
            return WRONG_DRIVER_VERSION;
    }
    // query chipset ID
    getParamIoctl(I915_PARAM_CHIPSET_ID, &chipset_id);
    getParamIoctl(I915_PARAM_REVISION, &revision_id);
    
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
    HWConfigTable = deviceBlob;
    if (deviceBlob) {
        printf("We have a hardware configuration table!\n");
    }
    descriptor = getDevInfoFromDescriptorTable(chipset_id);
    if (descriptor->pHwInfo) {
        descriptor->setupHardwareInfo(descriptor->pHwInfo);
        descriptor->pHwInfo->platform->usDeviceID = chipset_id;
        descriptor->pHwInfo->platform->usRevId = revision_id;
    }
    this->devName = descriptor->devName;
    SystemInfo* sysInfo = descriptor->pHwInfo->gtSystemInfo;

    LOG("\n");
    LOG("GPU %d\n", numDevices);
    LOG("------------------------------------------------------------------\n");
    LOG("Device ID: \t\t0x%X [%d]\n", this->chipset_id, this->revision_id);
    LOG("Device Name: \t\t%s\n", this->devName);

    // query topology info
    void* topology = queryIoctl(DRM_I915_QUERY_TOPOLOGY_INFO, 0u, 0);
    auto data = reinterpret_cast<drm_i915_query_topology_info*>(topology);
    if (!data) {
        return QUERY_FAILED;
    }

    //TODO: Add checks for topology info
    translateTopologyInfo(data, sysInfo);
    free(data);
    sysInfo->ThreadCount = sysInfo->EUCount * sysInfo->NumThreadsPerEu;

    // Soft-Pinning supported?
    getParamIoctl(I915_PARAM_HAS_EXEC_SOFTPIN, &supportsSoftPin);
    if (!supportsSoftPin) {
        return NO_SOFTPIN_SUPPORT;
    }

    // Enable Turbo Boost
    int ret = enableTurboBoost();
    /*
    if (ret) {
        return NO_TURBO_BOOST;
    }
    */

    // query memory info here

    // query engine info
    engines = queryIoctl(DRM_I915_QUERY_ENGINE_INFO, 0u, 0);
    if (!engines) {
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
    //getQueueSliceCount(device);

    // check if non-persistent contexts are supported. A non-persistent context gets
    // destroyed immediately upon closure (through DRM_I915_GEM_CONTEXT_CLOSE, fd destruction 
    // or process termination). A persistent context can finish the batch before closing.
    checkNonPersistentContextsSupport();

    checkPreemptionSupport();
    
    // Initialize memory manager here

    LOG("\n");
    return SUCCESS;
}

std::unique_ptr<DeviceDescriptor> Device::getDevInfoFromDescriptorTable(uint16_t chipset_id) {
    auto descriptor = std::make_unique<DeviceDescriptor>();
    for (auto &d : deviceDescriptorTable) {
        if (chipset_id == d.deviceId) {
            descriptor->pHwInfo = d.pHwInfo;
            descriptor->setupHardwareInfo = d.setupHardwareInfo;
            //device->eGtType = d.eGtType;
            descriptor->devName = d.devName;
            break;
        }
    }
    return descriptor;
}


bool Device::checkDriverVersion() {
    drm_version version = {0};
    char name[5] = {};
    version.name = name;
    version.name_len = 5;
    int ret = ioctl(fd, DRM_IOCTL_VERSION, &version);
    if (ret) {
        return false;
    }
    name[4] = '\0';
    //TODO: add driver info to gpuInfo struct
    strncpy(driver_name, name, 5);
    LOG("Kernel driver: \t\t%s\n", this->driver_name);
    return strcmp(name, "i915") == 0;
}

int Device::getParamIoctl(int param, int* paramValue) {
    drm_i915_getparam getParam = {0};
    getParam.param = param;
    getParam.value = paramValue;

    int ret = ioctl(fd, DRM_IOCTL_I915_GETPARAM, &getParam);
    return ret;
}

void* Device::queryIoctl(uint32_t queryId, uint32_t queryItemFlags, int32_t length) {
    drm_i915_query query = {0};
    drm_i915_query_item queryItem = {0};
    queryItem.query_id = queryId;
    queryItem.length = 0;
    queryItem.flags = queryItemFlags;
    query.items_ptr = reinterpret_cast<uint64_t>(&queryItem);
    query.num_items = 1;

    int ret = ioctl(fd, DRM_IOCTL_I915_QUERY, &query);
    if (ret != 0 || queryItem.length <= 0) {
        return nullptr;
    }

    void* data = malloc(queryItem.length);
    memset(data, 0, queryItem.length);
    queryItem.data_ptr = reinterpret_cast<uint64_t>(data);

    ret = ioctl(fd, DRM_IOCTL_I915_QUERY, &query);
    if (ret != 0 || queryItem.length <= 0) {
        return nullptr;
    }
    //printf("length: %d\n", queryItem.length);
    length = queryItem.length;
    return data;
}

void Device::translateTopologyInfo(drm_i915_query_topology_info* topologyInfo, SystemInfo* sysInfo) {
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
    LOG("Execution Units: \t%d\n", euCount);
    LOG("Slices: \t\t%d\n", sliceCount);
    LOG("Subslices per Slice: \t%d\n", subSliceCountPerSlice);
    LOG("EUs per Subslice: \t%d\n", euCountPerSubSlice);
    sysInfo->EUCount = euCount;
    sysInfo->SubSliceCount = subSliceCount;
    sysInfo->SliceCount = sliceCount;
    this->subSliceCountPerSlice = subSliceCountPerSlice;
    this->euCountPerSubSlice = euCountPerSubSlice;
}

int Device::createDrmVirtualMemory() {
    drm_i915_gem_vm_control ctl = {0};
    int ret = ioctl(fd, DRM_IOCTL_I915_GEM_VM_CREATE, &ctl);
    if (ret == 0) {
        if (ctl.vm_id == 0) {
            return -1;          // 0 is reserved for invalid/unassigned ppGTT
        }
        drmVmId = ctl.vm_id;
    }
    return ret;
}

int Device::enableTurboBoost() {
    // Even though my test machine (Skylake) has Turbo Boost 2.0, this does not work. 
    // Do we need to specify a context id first?
    // use ftrace to see why we get EINVAL error
    drm_i915_gem_context_param contextParam = {0};

    contextParam.param = I915_CONTEXT_PRIVATE_PARAM_BOOST;
    contextParam.value = 1;
    int ret = ioctl(fd, DRM_IOCTL_I915_GEM_CONTEXT_SETPARAM, &contextParam);
    return ret;
}

int Device::queryGttSize() {
    drm_i915_gem_context_param contextParam = {0};
    contextParam.ctx_id = 0;
    contextParam.param = I915_CONTEXT_PARAM_GTT_SIZE;
    contextParam.value = 0;
    int ret = ioctl(fd, DRM_IOCTL_I915_GEM_CONTEXT_GETPARAM, &contextParam);
    if (ret == 0) {
        gttSize = contextParam.value;
    }
    return ret;
}

void Device::checkNonPersistentContextsSupport() {
    drm_i915_gem_context_param contextParam = {0};
    contextParam.param = I915_CONTEXT_PARAM_PERSISTENCE;

    int ret = ioctl(fd, DRM_IOCTL_I915_GEM_CONTEXT_GETPARAM, &contextParam);
    if (ret == 0 && contextParam.value == 1) {
        nonPersistentContextsSupported = true;
    }
    else {
        nonPersistentContextsSupported = false;
    }
}

bool Device::getNonPersistentContextsSupported() {
    return nonPersistentContextsSupported;
}

DeviceDescriptor* Device::getDeviceDescriptor() {
    return descriptor.get();
}

void Device::checkPreemptionSupport() {
    int value = 0;
    int ret = getParamIoctl(I915_PARAM_HAS_SCHEDULER, &value);
    schedulerValue = value;
    preemptionSupported = ((0 == ret) && (value & I915_SCHEDULER_CAP_PREEMPTION));
}



std::vector<int> openDevices(int* err) {
    const char* pciDevicesDirectory = "/dev/dri/by-path";
    std::vector<std::string> files;
    std::vector<int> deviceIDs;
    DIR* dir = opendir(pciDevicesDirectory);
    if (!dir) {
        *err = NO_DEVICES_FOUND;
        return deviceIDs;
    }
    dirent* entry = nullptr;
    while((entry = readdir(dir)) != nullptr) {
        if (entry->d_name[0] == '.')
            continue;
        std::string fullPath = pciDevicesDirectory;
        fullPath += "/";
        fullPath += entry->d_name;
        files.push_back(fullPath);
    }
    closedir(dir);
    const char* renderSuffix = "-render";
    for (auto& file : files) {
        const char* path = file.c_str();
        const char* pos = strstr(path, renderSuffix);
        if (!pos)
            continue;
        uint32_t offset = pos - path;
        if (offset < strlen(path) - strlen(renderSuffix))
            continue;
        if (offset < 33 || path[offset - 13] != '-')
            continue;
        int fileDescriptor = open(path, O_RDWR | O_CLOEXEC);
        if (fileDescriptor == -1)
            continue;
        deviceIDs.push_back(fileDescriptor);
    }
    *err = SUCCESS;
    return deviceIDs;
}











