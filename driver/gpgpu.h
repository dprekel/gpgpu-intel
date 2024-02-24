struct gpuInfo {
    int fd;
    const char* driver_name;
    int chipset_id;
    int revision_id;

    uint16_t maxSliceCount;
    uint16_t maxSubSliceCount;
    uint16_t maxEUCount;
};

#define SUCCESS                              0
#define WRONG_DRIVER_VERSION                -1
#define FAILED_TOPOLOGY_QUERY               -2

extern int gpInitGPU(struct gpuInfo* gpuInfo);
//extern void* gpAllocateAndPinBuffer(size_t size);

