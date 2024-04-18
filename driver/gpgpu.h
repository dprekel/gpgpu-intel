#pragma once

struct GPU {
    int fd;
    const char* driver_name;
    int chipset_id;
    int revision_id;

    void* HWConfigTable;                    // if this is nullptr, it is not supported
    void* descriptor;

    uint16_t sliceCount;
    uint16_t subSliceCount;
    uint16_t euCount;
    uint16_t subSliceCountPerSlice;
    uint16_t euCountPerSubSlice;

    int supportsSoftPin;

    void* engines;                          // list of GPU engines (command streamers)

    int drmVmId;                            // unique identifier for ppGTT
    uint64_t gttSize;
    bool nonPersistentContextsSupported;
    bool preemptionSupported;
    int schedulerValue;

    void* context;
    void* kernel;
};

#define SUCCESS                              0
#define WRONG_DRIVER_VERSION                -1
#define QUERY_FAILED                        -2
#define NO_SOFTPIN_SUPPORT                  -3
#define NO_TURBO_BOOST                      -4
#define VM_CREATION_FAILED                  -5
#define CONTEXT_CREATION_FAILED             -6
#define BUFFER_ALLOCATION_FAILED            -7

extern int CreateDevice(GPU* gpuInfo);
extern int GetInfo(GPU* gpuInfo);
extern int CreateContext(GPU* gpuInfo);
extern int CreateBuffer(GPU* gpuInfo, void* buffer, size_t size);
extern int BuildKernel(GPU* gpuInfo, const char* filename, const char* options);
extern int EnqueueNDRangeKernel(GPU* gpuInfo);
