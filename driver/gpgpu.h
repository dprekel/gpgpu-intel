struct gpuInfo {
    int fd;
    const char* driver_name;
    int chipset_id;
    int revision_id;

    uint16_t sliceCount;
    uint16_t subSliceCount;
    uint16_t euCount;
    uint16_t subSliceCountPerSlice;
    uint16_t euCountPerSubSlice;

    void* HWConfigTable;                    // if this is nullptr, it is not supported

    int supportsSoftPin;

    void* engines;                          // list of GPU engines (command streamers)

    int drmVmId;                            // unique identifier for ppGTT
    uint64_t gttSize;
    bool nonPersistentContextsSupported;
    bool preemptionSupported;
    int schedulerValue;

    void* kernel;
};

#define SUCCESS                              0
#define WRONG_DRIVER_VERSION                -1
#define QUERY_FAILED                        -2
#define NO_SOFTPIN_SUPPORT                  -3
#define NO_TURBO_BOOST                      -4
#define VM_CREATION_FAILED                  -5

extern int gpInitGPU(struct gpuInfo* gpuInfo);
extern void logGPUInfo(struct gpuInfo* gpuInfo);
//extern void* gpAllocateAndPinBuffer(size_t size);
extern int gpBuildKernel(struct gpuInfo* gpuInfo, const char* filename, const char* options);

