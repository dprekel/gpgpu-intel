#include <gpgpu.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <chrono>
#include <limits>
#include <string>
#include <vector>

#define CLFFT_CB_SIZE  32

typedef union {
    uint32_t u;
    int i;
} cb_t;

int main() {
    int err = 0;
    std::vector<pDevice*> devices = CreateDevices(&err);
    printf("[DEBUG] CreateDevices: %d\n", err);

    pContext* context = CreateContext(devices, 0u, &err);
    printf("[DEBUG] CreateContext: %d\n", err);
    pKernel* kernel = BuildKernel(context, "fft1d.cl", nullptr, true, &err);
    printf("[DEBUG] BuildKernel: %d\n", err);


    // Constant params buffer
    size_t batchSize = 1;
    pBuffer* constBuffer = CreateBuffer(context, CLFFT_CB_SIZE * sizeof(int), &err);
    printf("[DEBUG] CreateBuffer: %d\n", err);
    if (err)
        return -1;
    cb_t constantBufferParams[CLFFT_CB_SIZE];
    memset(&constantBufferParams, 0, sizeof(constantBufferParams));
    constantBufferParams[0].u = std::max<uint32_t>(1, uint32_t(batchSize));
    cb_t* constBufferMem = (cb_t*)(constBuffer->mem);
    memcpy(constBufferMem, constantBufferParams, sizeof(constantBufferParams));

    // Input/Output buffer
    size_t N = 16;
    size_t size = 2*N*sizeof(float);
    pBuffer* input = CreateBuffer(context, size, &err);
    printf("[DEBUG] CreateBuffer: %d\n", err);
    if (err)
        return -1;
    float* inputMem = (float*)(input->mem);
    size_t print_iter = 0;
    while (print_iter < N) {
        float x = (float)print_iter;
        float y = (float)print_iter * 3;
        inputMem[2*print_iter  ] = x;
        inputMem[2*print_iter+1] = y;
        printf("(%f, %f) ", x, y);
        print_iter++;
    }
    printf("\n");


    err = SetKernelArg(kernel, 0, sizeof(constBuffer), static_cast<void*>(constBuffer));
    printf("[DEBUG] SetKernelArg: %d\n", err);
    err = SetKernelArg(kernel, 1, sizeof(input), static_cast<void*>(input));
    printf("[DEBUG] SetKernelArg: %d\n", err);
    // number of work items per work group dimension
    const size_t local[3] = {64, 1, 1};
    // total number of work items in each dimension
    const size_t global[3] = {64, 1, 1};

    for (int i = 0; i < 1; i++) {
        printf("\n");
        printf("[DEBUG] Starting Task %d\n", i+1);
        err = ExecuteKernel(context, kernel, 3, global, local);
        if (err) {
            printf("[DEBUG] Batchbuffer failed with %d\n", err);
        }
        /*
        printf("  matCMem[0] = %f\n", vecCMem[0]);
        printf("  matCMem[100] = %f\n", vecCMem[100]);
        printf("  matCMem[7500000] = %f\n", vecCMem[7500000]);
        printf("  matCMem[size-10] = %f\n", vecCMem[size - 10]);
        printf("  matCMem[size] = %f\n", vecCMem[size]);
        printf("[DEBUG] Checking validity ...  ");
        fflush(stdout);
        std::chrono::high_resolution_clock::time_point time1, time2;
        time1 = std::chrono::high_resolution_clock::now();
        bool valid = checkValidity(matAMem, matBMem, matCMem, size, size, true);
        time2 = std::chrono::high_resolution_clock::now();
        int64_t elapsedTime = std::chrono::duration_cast<std::chrono::nanoseconds>(time2 - time1).count();
        if (!valid) {
            printf("Failed! (Execution time: %.2f seconds)\n", elapsedTime/1e9);
        } else {
            printf("Passed! (Execution time: %.2f seconds)\n", elapsedTime/1e9);
        }
        for (size_t i = 0; i < size; i++) {
            vecCMem[i] = 0.0;
        }
        */
        print_iter = 0;
        while (print_iter < N) {
            printf("(%f, %f) ", inputMem[2 * print_iter], inputMem[2 * print_iter + 1]);
            print_iter++;
        }
    }
    printf("\n");

    err = ReleaseKernel(kernel);
    err = ReleaseContext(context);
    err = ReleaseBuffer(input);
    err = ReleaseBuffer(constBuffer);
    err = ReleaseDevice(devices, 0u);
    return 0;
}


/*
PATCH_TOKEN_MEDIA_INTERFACE_DESCRIPTOR_LOAD
PATCH_TOKEN_INTERFACE_DESCRIPTOR_DATA
PATCH_TOKEN_BINDING_TABLE_STATE
PATCH_TOKEN_ALLOCATE_LOCAL_SURFACE
PATCH_TOKEN_DATA_PARAMETER_BUFFER
PATCH_TOKEN_DATA_PARAMETER_BUFFER
PATCH_TOKEN_DATA_PARAMETER_BUFFER
PATCH_TOKEN_DATA_PARAMETER_BUFFER
PATCH_TOKEN_DATA_PARAMETER_BUFFER
PATCH_TOKEN_DATA_PARAMETER_BUFFER
PATCH_TOKEN_DATA_PARAMETER_BUFFER
PATCH_TOKEN_STATELESS_GLOBAL_MEMORY_OBJECT_KERNEL_ARGUMENT
PATCH_TOKEN_STATELESS_CONSTANT_MEMORY_OBJECT_KERNEL_ARGUMENT
PATCH_TOKEN_ALLOCATE_STATELESS_CONSTANT_MEMORY_SURFACE_WITH_INITIALIZATION
PATCH_TOKEN_ALLOCATE_STATELESS_PRIVATE_MEMORY
PATCH_TOKEN_DATA_PARAMETER_STREAM
PATCH_TOKEN_THREAD_PAYLOAD
PATCH_TOKEN_EXECUTION_ENVIRONMENT
PATCH_TOKEN_KERNEL_ATTRIBUTES_INFO
PATCH_TOKEN_KERNEL_ARGUMENT_INFO
PATCH_TOKEN_KERNEL_ARGUMENT_INFO


PATCH_TOKEN_MEDIA_INTERFACE_DESCRIPTOR_LOAD
PATCH_TOKEN_INTERFACE_DESCRIPTOR_DATA
PATCH_TOKEN_BINDING_TABLE_STATE
15
PATCH_TOKEN_DATA_PARAMETER_BUFFER
PATCH_TOKEN_DATA_PARAMETER_BUFFER
PATCH_TOKEN_DATA_PARAMETER_BUFFER
PATCH_TOKEN_DATA_PARAMETER_BUFFER
PATCH_TOKEN_DATA_PARAMETER_BUFFER
PATCH_TOKEN_DATA_PARAMETER_BUFFER
PATCH_TOKEN_DATA_PARAMETER_BUFFER
...



DATA_PARAMETER_GLOBAL_WORK_OFFSET
DATA_PARAMETER_GLOBAL_WORK_OFFSET
DATA_PARAMETER_GLOBAL_WORK_OFFSET
DATA_PARAMETER_LOCAL_WORK_SIZE
DATA_PARAMETER_LOCAL_WORK_SIZE
DATA_PARAMETER_LOCAL_WORK_SIZE
DATA_PARAMETER_BUFFER_STATEFUL
*/
















