#pragma once

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


class pDevice {
    int magic;
};


extern pDevice* CreateDevice(int* err);

extern int GetInfo(pDevice* device);

extern int CreateContext(pDevice* device);

extern int CreateBuffer(pDevice* device,
                        void* buffer,
                        size_t size);

extern int BuildKernel(pDevice* device,
                        const char* filename,
                        const char* options,
                        int architecture,
                        bool disassemble);

extern int EnqueueNDRangeKernel(pDevice* device,
                        uint32_t work_dim,
                        const size_t* global_work_offset,
                        const size_t* global_work_size,
                        const size_t* local_work_size);

extern int ReleaseObjects(pDevice* device);




