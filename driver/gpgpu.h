#pragma once

#include <stdint.h>
#include <stdio.h>

#include <vector>

#define SUCCESS                               0
#define UNSUPPORTED_KERNEL_DRIVER            -1
#define QUERY_FAILED                         -2
#define SOFTPIN_NOT_SUPPORTED                -3
#define UNSUPPORTED_HARDWARE                 -4
#define CONTEXT_ALREADY_EXISTS               -6
#define CONTEXT_CREATION_FAILED              -7
#define BUFFER_ALLOCATION_FAILED             -8
#define INVALID_WORK_GROUP_SIZE              -9
#define NO_DEVICE_ERROR                     -10
#define NO_CONTEXT_ERROR                    -11
#define NO_KERNEL_ERROR                     -12
#define NO_BUFFER_ERROR                     -13
#define FRONTEND_BUILD_ERROR                -14
#define BACKEND_BUILD_ERROR                 -15
#define SOURCE_LOAD_ERROR                   -16
#define COMPILER_LOAD_ERROR                 -17
#define INVALID_KERNEL                      -20
#define INVALID_KERNEL_ARG                  -19
#define UNSUPPORTED_PATCHTOKENS             -26
#define KERNEL_DUMP_ERROR                   -24
#define SIP_ERROR                           -21
#define GEM_EXECBUFFER_FAILED               -22
#define POST_SYNC_OPERATION_FAILED          -23


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
extern pContext* CreateContext(std::vector<pDevice*>& dev, 
                        size_t devIndex,
                        int* err);
extern pBuffer* CreateBuffer(pContext* context,
                        size_t size,
                        int* ret);
extern pKernel* BuildKernel(pContext* context,
                        const char* filename,
                        const char* options,
                        bool enableKernelDump,
                        int* err);
extern int SetKernelArg(pKernel* kern,
                        uint32_t arg_index,
                        size_t arg_size,
                        void* arg_value);
extern int ExecuteKernel(pContext* context,
                        pKernel* kernel,
                        uint32_t work_dim,
                        const size_t* global_work_size,
                        const size_t* local_work_size);
extern int ReleaseDevice(std::vector<pDevice*>& dev,
                        size_t devIndex);
extern int ReleaseContext(pContext* context);
extern int ReleaseKernel(pKernel* kernel);
extern int ReleaseBuffer(pBuffer* buffer);

