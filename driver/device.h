#pragma once

#include <stdint.h>

#include <memory>

#include "context.h"
#include "gpgpu.h"
#include "hwinfo.h"
#include "ioctl.h"
#include "kernel.h"

#ifdef DEBUG
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...) do {} while (0)
#endif

class Context;
class Kernel;

struct DeviceDescriptor {
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
    bool getNonPersistentContextsSupported();
    //TODO: Only one function that returns DeviceDescriptor
    //TODO: Either initialize values in initializer list or in class body, be consistent!
    //TODO: Try to make all member variables private
    std::unique_ptr<DeviceDescriptor> getDevInfoFromDescriptorTable(uint16_t chipset_id);
    DeviceDescriptor* getDeviceDescriptor();

    Context* context = nullptr;
    int fd;
    int drmVmId;                            // unique identifier for ppGTT
    std::unique_ptr<DeviceDescriptor> descriptor;
  private:
    bool checkDriverVersion();
    int getParamIoctl(int param, int* paramValue);
    void* queryIoctl(uint32_t queryId, uint32_t queryItemFlags, int32_t length);
    void translateTopologyInfo(drm_i915_query_topology_info* topologyInfo, SystemInfo* sysInfo);
    int createDrmVirtualMemory();
    int queryGttSize();
    void checkNonPersistentContextsSupport();
    void checkPreemptionSupport();
    int enableTurboBoost();

    const char* igcName;
    const char* fclName;
    CIFMain* igcMain = nullptr;
    CIFMain* fclMain = nullptr;

    uint32_t numDevices = 0;
    char driver_name[5];
    int chipset_id;
    int revision_id;

    void* HWConfigTable = nullptr;                  // if this is nullptr, it is not supported

    uint16_t subSliceCountPerSlice;
    uint16_t euCountPerSubSlice;

    int supportsSoftPin;
    void* engines = nullptr;                        // list of GPU engines (command streamers)
    uint64_t gttSize;
    bool nonPersistentContextsSupported;
    bool preemptionSupported;
    int schedulerValue;
};








