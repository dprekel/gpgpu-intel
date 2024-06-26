#pragma once

#include <stdint.h>
#include <stdio.h>

#include <vector>

#define SUCCESS                               0
#define WRONG_DRIVER_VERSION                 -1
#define QUERY_FAILED                         -2
#define NO_SOFTPIN_SUPPORT                   -3
#define NO_TURBO_BOOST                       -4
#define CONTEXT_ALREADY_EXISTS               -5
#define CONTEXT_CREATION_FAILED              -6
#define BUFFER_ALLOCATION_FAILED             -7
#define KERNEL_ALLOCATION_FAILED             -8
#define INVALID_WORK_GROUP_SIZE              -9
#define NO_DEVICE_ERROR                     -10
#define NO_CONTEXT_ERROR                    -11
#define NO_KERNEL_ERROR                     -12
#define NO_BUFFER_ERROR                     -13
#define FRONTEND_BUILD_ERROR                -14
#define BACKEND_BUILD_ERROR                 -15
#define LOAD_SOURCE_FAILED                  -16
#define NO_DEVICES_FOUND                    -17
#define COMPILER_LOAD_FAILED                -18
#define INVALID_KERNEL_FORMAT               -19
#define INVALID_KERNEL_ARG                  -20
#define INVALID_KERNEL                      -21
#define SIP_ERROR                           -22
#define GEM_EXECBUFFER_FAILED               -23


class pDevice {
  public:
    int magic = 0x373E5A13;
    const char* devName;
};

class pContext {
  public:
    int magic = 0x373E5A13;
};

class pBuffer {
  public:
    int magic = 0x373E5A13;
    void* mem;
};

class pKernel {
  public:
    int magic = 0x373E5A13;
};

extern std::vector<pDevice*> CreateDevices(int* err);
extern pContext* CreateContext(pDevice* device, 
                        int* err);
extern pBuffer* CreateBuffer(pContext* context,
                        size_t size,
                        int* ret);
extern pKernel* BuildKernel(pContext* context,
                        const char* filename,
                        const char* options,
                        uint16_t chipset_id,
                        bool enableDisassemble,
                        int* err);
extern int SetKernelArg(pKernel* kern,
                        uint32_t arg_index,
                        size_t arg_size,
                        void* arg_value);
extern int EnqueueNDRangeKernel(pContext* context,
                        pKernel* kernel,
                        uint32_t work_dim,
                        const size_t* global_work_size,
                        const size_t* local_work_size);
extern int ReleaseDevice(pDevice* device);
extern int ReleaseContext(pContext* context);
extern int ReleaseKernel(pKernel* kernel);
extern int ReleaseBuffer(pBuffer* buffer);

