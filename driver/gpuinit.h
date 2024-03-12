#pragma once

#include "hwinfo/hwinfo.h"


struct DeviceDescriptor {
    uint16_t deviceId;
    const HardwareInfo* pHwInfo;
    void (*setupHardwareInfo)(const HardwareInfo*);
    GTTYPE eGtType;
    const char* devName;
};

int openDeviceFile();
bool checkDriverVersion(struct gpuInfo* gpuInfo);
int getParamIoctl(struct gpuInfo* gpuInfo, int param, int* paramValue);
void* queryIoctl(struct gpuInfo* gpuInfo, uint32_t queryId, uint32_t queryItemFlags, int32_t length);
void translateTopologyInfo(struct gpuInfo* gpuInfo, struct drm_i915_query_topology_info* topologyInfo);
int createDrmVirtualMemory(struct gpuInfo* gpuInfo);
int queryGttSize(struct gpuInfo* gpuInfo);
void checkNonPersistentContextsSupport(struct gpuInfo* gpuInfo);
void checkPreemptionSupport(struct gpuInfo* gpuInfo);
int enableTurboBoost(struct gpuInfo* gpuInfo);

int allocUserptr(uintptr_t alloc, size_t size, uint64_t flags);









