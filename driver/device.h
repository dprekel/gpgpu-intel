#pragma once

#include "hwinfo.h"

struct GPU;

struct DeviceDescriptor {
    uint16_t deviceId;
    const HardwareInfo* pHwInfo;
    void (*setupHardwareInfo)(const HardwareInfo*);
    GTTYPE eGtType;
    const char* devName;
};

class Device {
  public:
    Device(GPU* gpuInfo);
    ~Device();
    int initializeGPU();
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
  private:
    GPU* gpuInfo;
};








