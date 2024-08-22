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
#include "gpgpu_api.h"
#include "hwinfo.h"
#include "ioctl.h"

extern const char* deviceExtensionsList;

const DeviceDescriptor deviceDescriptorTable[] = {
#define NAMEDDEVICE(devId, gt, devName) {devId, &gt::hwInfo, &gt::setupHardwareInfo, devName},
#define DEVICE(devId, gt) {devId, &gt::hwInfo, &gt::setupHardwareInfo, ""},
#include "devices_base.h"
#undef DEVICE
#undef NAMEDDEVICE
    {0, nullptr, nullptr}
};

Device::Device(int fd, CompilerInfo* compilerInfo)
         : fd(fd) {
    fclMain = compilerInfo->fclMain;
    igcMain = compilerInfo->igcMain;
}

Device::~Device() {
    DBG_LOG("[DEBUG] Device destructor called!\n");
    if (fclMain)
        fclMain->Release();
    if (igcMain)
        igcMain->Release();
    //TODO: Close file descriptor for device file
    //TODO: Make sure Device can only be deleted if all other objects are already deleted
}


std::vector<int> Device::openDevices(int* err) {
    const char* pciDevicesDirectory = "/dev/dri/by-path";
    std::vector<std::string> files;
    std::vector<int> deviceIDs;
    DIR* dir = opendir(pciDevicesDirectory);
    if (!dir) {
        *err = NO_DEVICE_ERROR;
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
        if (!fileDescriptor)
            continue;
        deviceIDs.push_back(fileDescriptor);
    }
    *err = SUCCESS;
    return deviceIDs;
}


CompilerInfo Device::initCompiler(int* ret) {
    CompilerInfo compilerInfo;
    *ret = Kernel::loadCompiler(compilerInfo.fclName, &compilerInfo.fclMain);
    if (*ret)
        return compilerInfo;
    *ret = Kernel::loadCompiler(compilerInfo.igcName, &compilerInfo.igcMain);
    return compilerInfo;
}

CIFMain* Device::getIgcMain() {
    return igcMain;
}

CIFMain* Device::getFclMain() {
    return fclMain;
}

DeviceDescriptor* Device::getDeviceDescriptor() {
    return deviceDescriptor.get();
}

std::string& Device::getDeviceExtensions() {
    return deviceExtensions;
}

bool Device::getMidThreadPreemptionSupport() {
    return isMidThreadLevelPreemptionSupported;
}


int Device::initialize() {
    numDevices += 1;
    // only i915 kernel driver is supported.
    if (numDevices == 1) {
        bool isi915 = checkDriverVersion();
        if (isi915 == false)
            return UNSUPPORTED_KERNEL_DRIVER;
    }
    // query device IDs.
    int ret = getParamIoctl(I915_PARAM_CHIPSET_ID, &deviceID);
    ret = getParamIoctl(I915_PARAM_REVISION, &revisionID);
    if (ret)
        return QUERY_FAILED;

    // Get device info from device descriptor table above.
    deviceDescriptor = getDeviceInfoFromDescriptorTable(deviceID);
    if (!deviceDescriptor)
        return UNSUPPORTED_HARDWARE;
    deviceDescriptor->setupHardwareInfo(deviceDescriptor->pHwInfo);
    deviceDescriptor->pHwInfo->platform->usDeviceID = deviceID;
    deviceDescriptor->pHwInfo->platform->usRevId = revisionID;

    // If supported, get additional device info from hardware config blob.
    getDeviceInfoFromHardwareConfigBlob();
    this->devName = deviceDescriptor->devName;
    SystemInfo* sysInfo = deviceDescriptor->pHwInfo->gtSystemInfo;
    setEdramSize(sysInfo);
    setLastLevelCacheSize(sysInfo);

    INFO_LOG("\n");
    INFO_LOG("[INFO] GPU %d\n", numDevices);
    INFO_LOG("------------------------------------------------------------------\n");
    INFO_LOG("[INFO] Device ID: \t\t0x%X [%d]\n", this->deviceID, this->revisionID);
    INFO_LOG("[INFO] Device Name: \t\t%s\n", this->devName);

    ret = retrieveTopologyInfo(sysInfo);
    if (ret)
        return ret;
    FeatureTable* featureTable = deviceDescriptor->pHwInfo->featureTable;
    setDeviceExtensions(featureTable);
    ret = checkPreemptionSupport(featureTable);
    if (ret)
        return ret;
    ret = initHeapAllocatorForSoftpinning();
    if (ret)
        return ret;

    INFO_LOG("\n");
    return SUCCESS;
}

void Device::setEdramSize(SystemInfo* sysInfo) {
    if (deviceID == 0x5926 || deviceID == 0x1927 || deviceID == 0x192D ||
        deviceID == 0x3EA8 || deviceID == 0x3EA6 || deviceID == 0x5927 ||
        deviceID == 0x5926) {
        sysInfo->EdramSizeInKb = 64 * MemoryConstants::kiloByte;
    }
    if (deviceID == 0x193B || deviceID == 0x193D) {
        sysInfo->EdramSizeInKb = 128 * MemoryConstants::kiloByte;
    }
}

void Device::setLastLevelCacheSize(SystemInfo* sysInfo) {
}

void Device::setDeviceExtensions(FeatureTable* featureTable) {
    if (featureTable->flags.ftrSupportsOcl30) {
        deviceExtensions += "-ocl-version=300 ";
    } else {
        deviceExtensions += "-ocl-version=210 ";
    }
    deviceExtensions += "-cl-disable-zebin ";
    //if (enableStatelessToStatefulWithOffset)
        deviceExtensions += "-cl-intel-has-buffer-offset-arg ";
    /*
    if (isForceEmuInt32DivRemSPWARequired)
        deviceExtensions += "-cl-intel-force-emu-sp-int32divrem ";
    */
    deviceExtensions += "-fpreserve-vec3-type ";
    deviceExtensions += "-cl-ext=-all";
    deviceExtensions.append(deviceExtensionsList);
    if (featureTable->flags.ftrSupportsOcl21) {
        if (featureTable->flags.ftrSupportsIndependentForwardProgress)
            deviceExtensions += ",+cl_khr_subgroups";
        if (featureTable->flags.ftrSVM)
            deviceExtensions += ",+cl_intel_spirv_device_side_avc_motion_estimation";
        deviceExtensions += ",+cl_intel_spirv_subgroups";
        deviceExtensions += ",+cl_khr_spirv_no_integer_wrap_decoration";
        deviceExtensions += ",+cl_intel_unified_shared_memory_preview";
    }
    if (featureTable->flags.ftrSVM) {
        deviceExtensions += ",+cl_intel_motion_estimation";
        deviceExtensions += ",+cl_intel_device_side_avc_motion_estimation";
    }
    //if (supportsAdvancedVme)
        deviceExtensions += ",+cl_intel_advanced_motion_estimation";
    if (featureTable->flags.ftrSupportsInteger64BitAtomics) {
        deviceExtensions += ",+cl_khr_int64_base_atomics";
        deviceExtensions += ",+cl_khr_int64_extended_atomics";
    }
    //if (isPciBusInfoValid)
    //TODO: What is pci_bus_info?
        deviceExtensions += ",+cl_khr_pci_bus_info";
}


bool Device::checkDriverVersion() {
    drm_version version = {0};
    char name[5] = {};
    version.name = name;
    version.name_len = 5;
    int ret = ioctl(fd, DRM_IOCTL_VERSION, &version);
    if (ret)
        return false;
    name[4] = '\0';
    strncpy(driver_name, name, 5);
    INFO_LOG("[INFO] Kernel driver: \t\t%s\n", this->driver_name);
    return strcmp(name, "i915") == 0;
}

std::unique_ptr<DeviceDescriptor> Device::getDeviceInfoFromDescriptorTable(uint16_t deviceID) {
    auto descriptor = std::make_unique<DeviceDescriptor>();
    memset(descriptor.get(), 0x0, sizeof(DeviceDescriptor));
    for (auto &d : deviceDescriptorTable) {
        if (deviceID == d.deviceId) {
            descriptor->pHwInfo = d.pHwInfo;
            descriptor->setupHardwareInfo = d.setupHardwareInfo;
            descriptor->devName = d.devName;
            break;
        }
    }
    if (!descriptor->pHwInfo)
        return nullptr;
    return descriptor;
}

void Device::getDeviceInfoFromHardwareConfigBlob() {
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
    auto hardwareConfigBlob = queryIoctl(DRM_I915_QUERY_HWCONFIG_TABLE, 0u, 0u);
    if (hardwareConfigBlob)
        this->isHardwareConfigBlobSupported = true;
}

int Device::retrieveTopologyInfo(SystemInfo* sysInfo) {
    //TODO: This function has a serious issue with GEN11: total subSliceCount * EUsPerSubSlice
    //      doesn't give total number of EUs
    auto data = queryIoctl(DRM_I915_QUERY_TOPOLOGY_INFO, 0u, 0);
    auto topologyInfo = reinterpret_cast<drm_i915_query_topology_info*>(data.get());
    if (!topologyInfo)
        return QUERY_FAILED;

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
    INFO_LOG("[INFO] Execution Units: \t%d\n", euCount);
    INFO_LOG("[INFO] Slices: \t\t\t%d\n", sliceCount);
    INFO_LOG("[INFO] Subslices per Slice: \t%d\n", subSliceCountPerSlice);
    INFO_LOG("[INFO] EUs per Subslice: \t%d\n", euCountPerSubSlice);
    sysInfo->EUCount = euCount;
    sysInfo->SubSliceCount = subSliceCount;
    sysInfo->SliceCount = sliceCount;
    sysInfo->ThreadCount = sysInfo->EUCount * sysInfo->NumThreadsPerEu;
    this->subSliceCountPerSlice = subSliceCountPerSlice;
    this->euCountPerSubSlice = euCountPerSubSlice;

    return SUCCESS;
}

int Device::initHeapAllocatorForSoftpinning() {
    // Soft-Pinning means that the kernel driver doesn't relocate buffer objects (BOs) that 
    // have been passed over from userspace. The kernel driver looks up the physical address
    // of the BO in the page table of the current CPU process and uses that to create a PTE 
    // in the GPU ppGTT.
    // Alternatively, userspace can pass in an address that the BO should be located at. The
    // kernel then looks up the current location using the BO handle and will do its utmost
    // to fit the BO into that new location. Softpinning a BO to a new location only works if
    // the address of that new location is a positive offset from the Graphics Base Address.
    int softPinSupported = 0;
    int ret = getParamIoctl(I915_PARAM_HAS_EXEC_SOFTPIN, &softPinSupported);
    if (ret)
        return QUERY_FAILED;
    if (!softPinSupported)
        return SOFTPIN_NOT_SUPPORTED;

    // Query GTT (Graphics Translation Table) size.
    drm_i915_gem_context_param contextParam = {0};
    contextParam.param = I915_CONTEXT_PARAM_GTT_SIZE;
    ret = ioctl(fd, DRM_IOCTL_I915_GEM_CONTEXT_GETPARAM, &contextParam);
    if (ret)
        return QUERY_FAILED;
    uint64_t GTTSize = contextParam.value;

    // Calculate Graphics Base Address.
    uint64_t gpuAddressSpace = GTTSize - 1;
    uint64_t gfxBase = 0x0;
    uint64_t gfxHeapSize = 4 * MemoryConstants::gigaByte;
    if (gpuAddressSpace == maxNBitValue(MemoryConstants::cpuVirtualAddressSize)) {
        gfxBase = maxNBitValue(48 - 1) + 1;
        heapAllocator.gpuBaseAddress = gfxBase + gfxHeapSize;
    } else {
        return UNSUPPORTED_HARDWARE;
    }
    // Initialize Heap Allocator.
    heapAllocator.pLeftBound = heapAllocator.gpuBaseAddress + MemoryConstants::megaByte;
    heapAllocator.pRightBound = heapAllocator.gpuBaseAddress + gfxHeapSize - MemoryConstants::pageSize64k;
    heapAllocator.availableSize = heapAllocator.pRightBound - heapAllocator.pLeftBound;
    heapAllocator.allocatedSize = 0u;

    return SUCCESS;
}

int Device::allocateHeapMemoryForSoftpinning(BufferObject* bo) {
    bo->gpuBaseAddress = canonize(heapAllocator.gpuBaseAddress);
    size_t sizeToAllocate = alignUp(bo->size, MemoryConstants::pageSize);
    if (heapAllocator.pLeftBound + sizeToAllocate <= heapAllocator.pRightBound) {
        heapAllocator.pRightBound -= sizeToAllocate;        
        heapAllocator.availableSize -= sizeToAllocate;
        heapAllocator.allocatedSize += sizeToAllocate;
        bo->gpuAddress = canonize(heapAllocator.pRightBound);
    } else {
        // Very unlikely that this happens. You would need to allocate 4 GB of memory for
        // kernel instructions and indirect data.
        return BUFFER_ALLOCATION_FAILED;
    }
    return SUCCESS;
}

int Device::getParamIoctl(int param, int* paramValue) {
    drm_i915_getparam getParam = {0};
    getParam.param = param;
    getParam.value = paramValue;
    int ret = ioctl(fd, DRM_IOCTL_I915_GETPARAM, &getParam);
    return ret;
}

std::unique_ptr<uint8_t[]> Device::queryIoctl(uint32_t queryId, uint32_t queryItemFlags, int32_t length) {
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
    auto data = std::make_unique<uint8_t[]>(queryItem.length);
    memset(data.get(), 0x0, queryItem.length);
    queryItem.data_ptr = reinterpret_cast<uint64_t>(data.get());

    ret = ioctl(fd, DRM_IOCTL_I915_QUERY, &query);
    if (ret != 0 || queryItem.length <= 0) {
        return nullptr;
    }
    length = queryItem.length;
    return data;
}

int Device::checkPreemptionSupport(FeatureTable* featureTable) {
    int schedulerFeature = 0;
    int ret = getParamIoctl(I915_PARAM_HAS_SCHEDULER, &schedulerFeature);
    if (ret)
        return QUERY_FAILED;
    if ((schedulerFeature & I915_SCHEDULER_CAP_PREEMPTION) &&
        featureTable->flags.ftrGpGpuMidThreadLevelPreempt) {
        this->isMidThreadLevelPreemptionSupported = true;
    }
    return SUCCESS;
}





