#pragma once

#include "hwinfo/hwinfo.h"

struct GPU;

struct DeviceDescriptor {
    uint16_t deviceId;
    const HardwareInfo* pHwInfo;
    void (*setupHardwareInfo)(const HardwareInfo*);
    GTTYPE eGtType;
    const char* devName;
};

int openDeviceFile();
bool checkDriverVersion(GPU* gpuInfo);
int getParamIoctl(GPU* gpuInfo, int param, int* paramValue);
void* queryIoctl(GPU* gpuInfo, uint32_t queryId, uint32_t queryItemFlags, int32_t length);
void translateTopologyInfo(GPU* gpuInfo, struct drm_i915_query_topology_info* topologyInfo);
int createDrmVirtualMemory(GPU* gpuInfo);
int queryGttSize(GPU* gpuInfo);
void checkNonPersistentContextsSupport(GPU* gpuInfo);
void checkPreemptionSupport(GPU* gpuInfo);
int enableTurboBoost(GPU* gpuInfo);

int allocUserptr(uintptr_t alloc, size_t size, uint64_t flags);









