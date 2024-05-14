#pragma once

#include <stdint.h>
#include <memory>

#include "hwinfo.h"
#include "ioctl.h"
#include "gpgpu.h"
#include "kernel.h"
#include "context.h"

class Context;
class Kernel;

struct DeviceDescriptor {
    /*
    DeviceDescriptor() {};
    ~DeviceDescriptor() {
        printf("DeviceDescriptor destructor called!\n");
    }
    */
    uint16_t deviceId;
    const HardwareInfo* pHwInfo;
    void (*setupHardwareInfo)(const HardwareInfo*);
    const char* devName;
};

std::vector<int> openDevices(int* err);

class Device : public pDevice {
  public:
    Device(int fd);
    ~Device();
    int initialize();
    int openDeviceFile();
    bool checkDriverVersion();
    int getParamIoctl(int param, int* paramValue);
    void* queryIoctl(uint32_t queryId, uint32_t queryItemFlags, int32_t length);
    void translateTopologyInfo(drm_i915_query_topology_info* topologyInfo, SystemInfo* sysInfo);
    int createDrmVirtualMemory();
    int queryGttSize();
    void checkNonPersistentContextsSupport();
    void checkPreemptionSupport();
    int enableTurboBoost();
    bool getNonPersistentContextsSupported();
    std::unique_ptr<DeviceDescriptor> getDevInfoFromDescriptorTable(uint16_t chipset_id);
    DeviceDescriptor* getDeviceDescriptor();

    Context* context;
    int fd;
    int drmVmId;                            // unique identifier for ppGTT
    char driver_name[5];
    int chipset_id;
    int revision_id;

    void* HWConfigTable;                    // if this is nullptr, it is not supported
    std::unique_ptr<DeviceDescriptor> descriptor;

    uint16_t sliceCount;
    uint16_t subSliceCount;
    uint16_t euCount;
    uint16_t subSliceCountPerSlice;
    uint16_t euCountPerSubSlice;

    int supportsSoftPin;

    void* engines;                          // list of GPU engines (command streamers)

    uint64_t gttSize;
    bool nonPersistentContextsSupported;
    bool preemptionSupported;
    int schedulerValue;
  private:
};








