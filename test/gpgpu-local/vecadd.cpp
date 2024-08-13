#include <gpgpu.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <chrono>
#include <limits>
#include <string>
#include <vector>


int main() {
    int err = 0;
    std::vector<pDevice*> devices = CreateDevices(&err);
    printf("[DEBUG] CreateDevices: %d\n", err);

    pContext* context = CreateContext(devices, 0u, &err);
    printf("[DEBUG] CreateContext: %d\n", err);
    pKernel* kernel = BuildKernel(context, "vecadd.cl", nullptr, true, &err);
    printf("[DEBUG] BuildKernel: %d\n", err);

    size_t size = 3968 * 3968;
    size_t vector_size = size*sizeof(float);
    
    pBuffer* vector_A = CreateBuffer(context, vector_size, &err);
    printf("[DEBUG] CreateBuffer: %d\n", err);
    if (err)
        return -1;
    pBuffer* vector_B = CreateBuffer(context, vector_size, &err);
    printf("[DEBUG] CreateBuffer: %d\n", err);
    if (err)
        return -1;
    pBuffer* vector_C = CreateBuffer(context, vector_size, &err);
    printf("[DEBUG] CreateBuffer: %d\n", err);
    if (err)
        return -1;

    float* vecAMem = (float*)(vector_A->mem);
    float* vecBMem = (float*)(vector_B->mem);
    float* vecCMem = (float*)(vector_C->mem);
    for (size_t i = 0; i < size; i++) {
        vecAMem[i] = 1.0;
        vecBMem[i] = 1.0;
    }

    err = SetKernelArg(kernel, 0, sizeof(vector_A), static_cast<void*>(vector_A));
    err = SetKernelArg(kernel, 1, sizeof(vector_B), static_cast<void*>(vector_B));
    err = SetKernelArg(kernel, 2, sizeof(vector_C), static_cast<void*>(vector_C));
    err = SetKernelArg(kernel, 3, vector_size, nullptr);
    err = SetKernelArg(kernel, 4, vector_size, nullptr);
    printf("[DEBUG] SetKernelArg: %d\n", err);
    // number of work items per work group dimension
    const size_t local[3] = {256, 1, 1};
    // total number of work items in each dimension
    const size_t global[3] = {15745024, 1, 1};

    for (int i = 0; i < 10; i++) {
        printf("\n");
        printf("[DEBUG] Starting Task %d\n", i+1);
        err = ExecuteKernel(context, kernel, 3, global, local);
        if (err) {
            printf("[DEBUG] Batchbuffer failed with %d\n", err);
        }
        printf("  matCMem[0] = %f\n", vecCMem[0]);
        printf("  matCMem[100] = %f\n", vecCMem[100]);
        printf("  matCMem[7500000] = %f\n", vecCMem[7500000]);
        printf("  matCMem[size-10] = %f\n", vecCMem[size - 10]);
        printf("  matCMem[size] = %f\n", vecCMem[size]);
        /*
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
        */
        for (size_t i = 0; i < size; i++) {
            vecCMem[i] = 0.0;
        }
    }
    printf("\n");

    err = ReleaseKernel(kernel);
    err = ReleaseContext(context);
    err = ReleaseBuffer(vector_A);
    err = ReleaseBuffer(vector_B);
    err = ReleaseBuffer(vector_C);
    err = ReleaseDevice(devices, 0u);
    return 0;
}









