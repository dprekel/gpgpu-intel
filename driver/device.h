#pragma once

#include <stdint.h>

#include "hwinfo.h"
#include "gpgpu.h"
#include "kernel.h"
#include "context.h"

class Context;
class Kernel;

struct DeviceDescriptor {
    uint16_t deviceId;
    const HardwareInfo* pHwInfo;
    void (*setupHardwareInfo)(const HardwareInfo*);
    GTTYPE eGtType;
    const char* devName;
};

class Device : public pDevice {
  public:
    Device();
    ~Device();
    int initialize();
    int openDeviceFile();
    bool checkDriverVersion();
    int getParamIoctl(int param, int* paramValue);
    void* queryIoctl(uint32_t queryId, uint32_t queryItemFlags, int32_t length);
    void translateTopologyInfo(struct drm_i915_query_topology_info* topologyInfo);
    int createDrmVirtualMemory();
    int queryGttSize();
    void checkNonPersistentContextsSupport();
    void checkPreemptionSupport();
    int enableTurboBoost();
    bool getNonPersistentContextsSupported();
    DeviceDescriptor* getDeviceDescriptor();

    Context* context;
    Kernel* kernel;
    int fd;
    int drmVmId;                            // unique identifier for ppGTT
    const char* driver_name;
    int chipset_id;
    int revision_id;

    void* HWConfigTable;                    // if this is nullptr, it is not supported
    DeviceDescriptor* descriptor;

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








