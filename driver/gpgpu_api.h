#pragma once

/**
 * @cond
 */

#include <stdint.h>
#include <stdio.h>

#include <vector>

#define SUCCESS                               0
#define UNSUPPORTED_KERNEL_DRIVER            -1
#define QUERY_FAILED                         -2
#define SOFTPIN_NOT_SUPPORTED                -3
#define UNSUPPORTED_HARDWARE                 -4
#define CONTEXT_ALREADY_EXISTS               -5
#define CONTEXT_CREATION_FAILED              -6
#define BUFFER_ALLOCATION_FAILED             -7
#define INVALID_WORK_GROUP_SIZE              -8
#define INVALID_WORK_SIZE                    -9
#define NO_DEVICE_ERROR                     -10
#define NO_CONTEXT_ERROR                    -11
#define NO_KERNEL_ERROR                     -12
#define NO_BUFFER_ERROR                     -13
#define FRONTEND_BUILD_ERROR                -14
#define BACKEND_BUILD_ERROR                 -15
#define SOURCE_LOAD_ERROR                   -16
#define COMPILER_LOAD_ERROR                 -17
#define INVALID_KERNEL                      -18
#define INVALID_KERNEL_ARG                  -19
#define UNSUPPORTED_PATCHTOKENS             -20
#define KERNEL_DUMP_ERROR                   -21
#define SIP_ERROR                           -22
#define GEM_EXECBUFFER_FAILED               -23
#define POST_SYNC_OPERATION_FAILED          -24

/**
 * @endcond
 */

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

/**
 * @brief Initializes all GPU devices.
 *
 * @param err A pointer to an integer containing the returned error code:
 * - SUCCESS
 * - NO_DEVICE_ERROR: GPU device file could not be found.
 * - UNSUPPORTED_HARDWARE: GPU is not in the list of supported device IDs.
 * - QUERY_FAILED: IOCTL to query device information failed.
 *
 * @return std::vector<pDevice*> A vector of pointers to pDevice objects.
 *
 */
extern std::vector<pDevice*> CreateDevices(int* err);


/**
 * @brief Creates a DRM context for one GPU device
 *
 * @param dev The list of devices returned by CreateDevices().
 * @param devIndex Index within dev list to select the GPU for which the context will be created
 * @param err A pointer to an integer containing the returned error code:
 *
 * @return pContext* A pointer to a pContext object.
 *
 */
extern pContext* CreateContext(std::vector<pDevice*>& dev, 
                        size_t devIndex,
                        int* err);


/**
 * @brief Creates a buffer object.
 *
 * This function allocates the specified amount of bytes on the heap. It will be page aligned.
 * The allocated memory will be mapped into GPU address space so it can be accessed by the GPU.
 * The pointer to the allocated memory is stored in the returned pBuffer object (mem variable).
 * If err is not NULL, accessing this pointer leads to undefined behaviour.
 *
 * @param context A pointer to a pContext object.
 * @param size The size in bytes of the buffer memory object to be allocated.
 * @param err A pointer to an integer containing the returned error code:
 * - SUCCESS: the function executed successfully.
 * - BUFFER_ALLOCATION_FAILED: malloc failure or failure while passing the malloc pointer to the kernel driver.
 * - GEM_EXECBUFFER_FAILED: Failure while mapping the allocated memory into GPU address space.
 *
 * @return pBuffer* A pointer to a pBuffer object.
 *
 */
extern pBuffer* CreateBuffer(pContext* context,
                        size_t size,
                        int* err);


/**
 * @brief Compiles a program written in OpenCL C language into a GPU executable.
 *
 * @param context A pointer to a pContext object.
 * @param filename A pointer to a null-terminated character string containing the file name of the program.
 * @param options A pointer to a null-terminated character string containing additional build options. If options is nullptr, it is ignored.
 * @param enableKernelDump If set to true, a copy of the executable binary (filename.isabin) will be stored in the /tmp directory.
 * @param err A pointer to an integer containing the returned error code:
 *
 * @return pKernel* A pointer to a pKernel object.
 *
 */
extern pKernel* BuildKernel(pContext* context,
                        const char* filename,
                        const char* options,
                        bool enableKernelDump,
                        int* err);


/**
 * @brief
 *
 * @param kernel A pointer to a pKernel object.
 * @param arg_index
 * @param arg_size
 * @param arg_value
 *
 * @return int An integer containing the returned error code:
 *
 */
extern int SetKernelArg(pKernel* kernel,
                        uint32_t arg_index,
                        size_t arg_size,
                        void* arg_value);


/**
 * @brief
 *
 * @param context A pointer to a pContext object.
 * @param kernel A pointer to a pKernel object.
 * @param
 * @param
 * @param
 *
 * @return
 *
 */
extern int ExecuteKernel(pContext* context,
                        pKernel* kernel,
                        uint32_t work_dim,
                        const size_t* global_work_size,
                        const size_t* local_work_size);


/**
 * @brief
 *
 * @param
 * @param
 *
 * @return
 *
 */
extern int ReleaseDevice(std::vector<pDevice*>& dev,
                        size_t devIndex);


extern int ReleaseContext(pContext* context);


extern int ReleaseKernel(pKernel* kernel);


extern int ReleaseBuffer(pBuffer* buffer);




