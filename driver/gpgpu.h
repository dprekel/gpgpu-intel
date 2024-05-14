#pragma once

#include <vector>

#define SUCCESS                               0
#define WRONG_DRIVER_VERSION                 -1
#define QUERY_FAILED                         -2
#define NO_SOFTPIN_SUPPORT                   -3
#define NO_TURBO_BOOST                       -4
#define VM_CREATION_FAILED                   -5
#define CONTEXT_CREATION_FAILED              -6
#define BUFFER_ALLOCATION_FAILED             -7
#define KERNEL_ALLOCATION_FAILED             -8
#define PREEMPTION_ALLOCATION_FAILED         -9
#define INVALID_WORK_GROUP_SIZE             -10
#define NO_DEVICE_ERROR                     -11
#define NO_CONTEXT_ERROR                    -12
#define NO_KERNEL_ERROR                     -13
#define FRONTEND_BUILD_ERROR                -14
#define BACKEND_BUILD_ERROR                 -15
#define LOAD_SOURCE_FAILED                  -16
#define NO_DEVICES_FOUND                    -17


class pDevice {
  public:
    const char* devName;
};

class pContext {
  public:
    int magic;
};

class pKernel {
  public:
    int magic;
};

extern std::vector<pDevice*> CreateDevices(int* err);
extern int GetInfo(pDevice* device);
extern pContext* CreateContext(pDevice* device, 
                        int* err);
extern int CreateBuffer(pContext* context,
                        void* buffer,
                        size_t size);
extern pKernel* BuildKernel(pContext* context,
                        const char* filename,
                        const char* options,
                        uint16_t chipset_id,
                        bool enableDisassemble,
                        int* err);
extern int EnqueueNDRangeKernel(pContext* context,
                        pKernel* kernel,
                        uint32_t work_dim,
                        const size_t* global_work_offset,
                        const size_t* global_work_size,
                        const size_t* local_work_size);
extern int ReleaseDevice(pDevice* device);
extern int ReleaseContext(pContext* context);
extern int ReleaseKernel(pKernel* kernel);

