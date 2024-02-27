struct gpuInfo {
    int fd;
    const char* driver_name;
    int chipset_id;
    int revision_id;

    uint16_t maxSliceCount;
    uint16_t maxSubSliceCount;
    uint16_t maxEUCount;

    // if this is nullptr, it is not supported
    void* HWConfigTable;

    int supportsSoftPin;
    uint64_t gttSize;
};

#define SUCCESS                              0
#define WRONG_DRIVER_VERSION                -1
#define QUERY_FAILED                        -2
#define NO_SOFTPIN_SUPPORT                  -3
#define NO_TURBO_BOOST                      -4

extern int gpInitGPU(struct gpuInfo* gpuInfo);
//extern void* gpAllocateAndPinBuffer(size_t size);

