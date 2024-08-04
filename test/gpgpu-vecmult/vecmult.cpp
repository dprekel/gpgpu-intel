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
    pKernel* kernel = BuildKernel(context, "vecmult.cl", nullptr, true, &err);
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

    float* vecAMem = (float*)(vector_A->mem);
    float* vecBMem = (float*)(vector_B->mem);
    for (size_t i = 0; i < size; i++) {
        vecAMem[i] = 2.0;
        vecBMem[i] = 0.0;
    }

    err = SetKernelArg(kernel, 0, sizeof(vector_A), static_cast<void*>(vector_A));
    err = SetKernelArg(kernel, 1, sizeof(vector_B), static_cast<void*>(vector_B));
    printf("[DEBUG] SetKernelArg: %d\n", err);
    // number of work items per work group dimension
    const size_t local[3] = {32, 1, 1};
    // total number of work items in each dimension
    const size_t global[3] = {15745024, 1, 1};

    for (int i = 0; i < 10; i++) {
        printf("\n");
        printf("[DEBUG] Starting Task %d\n", i+1);
        err = ExecuteKernel(context, kernel, 3, global, local);
        if (err) {
            printf("[DEBUG] Batchbuffer failed with %d\n", err);
        }
        printf("  matBMem[0] = %f\n", vecBMem[0]);
        printf("  matBMem[100] = %f\n", vecBMem[100]);
        printf("  matBMem[7500000] = %f\n", vecBMem[7500000]);
        printf("  matBMem[size-10] = %f\n", vecBMem[size - 10]);
        printf("  matBMem[size] = %f\n", vecBMem[size]);
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
            vecBMem[i] = 0.0;
        }
    }
    printf("\n");

    err = ReleaseKernel(kernel);
    err = ReleaseContext(context);
    err = ReleaseBuffer(vector_A);
    err = ReleaseBuffer(vector_B);
    err = ReleaseDevice(devices, 0u);
    return 0;
}









