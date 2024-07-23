#include <gpgpu.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <chrono>
#include <limits>
#include <string>
#include <vector>

#define TILE_SIZE_M     1
#define TILE_GROUP_M    16
#define TILE_SIZE_N     128
#define TILE_GROUP_N    1

bool checkValidity(const float* A, const float* B, const float* C, size_t size, size_t ldabc, bool Btransposed);

int main() {
    int err = 0;
    std::vector<pDevice*> devices = CreateDevices(&err);

    std::string build_options = "-DTILE_SIZE_M=" + std::to_string(TILE_SIZE_M)
                              + " -DTILE_GROUP_M=" + std::to_string(TILE_GROUP_M)
                              + " -DTILE_SIZE_N=" + std::to_string(TILE_SIZE_N)
                              + " -DTILE_GROUP_N=" + std::to_string(TILE_GROUP_N);
    pContext* context = CreateContext(devices, 0u, &err);
    pKernel* kernel = BuildKernel(context, "matmul.cl", build_options.c_str(), true, &err);

    size_t size = 3968;
    size_t matrix_memory_size = size*size*sizeof(float);
    
    pBuffer* matrix_A = CreateBuffer(context, matrix_memory_size, &err);
    pBuffer* matrix_B = CreateBuffer(context, matrix_memory_size, &err);
    pBuffer* matrix_C = CreateBuffer(context, matrix_memory_size, &err);
    // Initialize matrices A and B with ones
    size_t matrix_size = size*size;
    float* matAMem = (float*)(matrix_A->mem);
    float* matBMem = (float*)(matrix_B->mem);
    float* matCMem = (float*)(matrix_C->mem);
    for (size_t i = 0; i < matrix_size; i++) {
        matAMem[i] = 1.0;
        matBMem[i] = 1.0;
    }

    err = SetKernelArg(kernel, 0, sizeof(matrix_A), static_cast<void*>(matrix_A));
    err = SetKernelArg(kernel, 1, sizeof(int),      static_cast<void*>(&size));
    err = SetKernelArg(kernel, 2, sizeof(matrix_B), static_cast<void*>(matrix_B));
    err = SetKernelArg(kernel, 3, sizeof(int),      static_cast<void*>(&size));
    err = SetKernelArg(kernel, 4, sizeof(matrix_C), static_cast<void*>(matrix_C));
    err = SetKernelArg(kernel, 5, sizeof(int),      static_cast<void*>(&size));
    err = SetKernelArg(kernel, 6, sizeof(int),      static_cast<void*>(&size));
    // number of work items per work group dimension
    const size_t local[2] = {TILE_GROUP_M, TILE_GROUP_N};
    // total number of work items in each dimension
    const size_t global[2] = {size/TILE_SIZE_M, size/TILE_SIZE_N};

    for (int i = 0; i < 10; i++) {
        printf("\n");
        printf("[DEBUG] Starting Task %d\n", i+1);
        err = ExecuteKernel(context, kernel, 2, global, local);
        if (err) {
            printf("[DEBUG] Batchbuffer failed with %d\n", err);
        }
        /*
        printf("  matCMem[0] = %f\n", matCMem[0]);
        printf("  matCMem[size*100] = %f\n", matCMem[size *100]);
        printf("  matCMem[7500000] = %f\n", matCMem[7500000]);
        printf("  matCMem[matrix_size-10] = %f\n", matCMem[matrix_size - 10]);
        printf("  matCMem[matrix_size] = %f\n", matCMem[matrix_size]);
        */
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
        for (size_t i = 0; i < matrix_size; i++) {
            matCMem[i] = 0.0;
        }
    }
    printf("\n");

    err = ReleaseKernel(kernel);
    err = ReleaseContext(context);
    err = ReleaseBuffer(matrix_A);
    err = ReleaseBuffer(matrix_B);
    err = ReleaseBuffer(matrix_C);
    err = ReleaseDevice(devices, 0u);
    return 0;
}


bool checkValidity(const float* A, const float* B, const float* C, size_t size, size_t ldabc, bool Btransposed) {
    size_t lstride = Btransposed ? ldabc : 1;
    size_t jstride = Btransposed ? 1 : ldabc;

    float max_value = 1;
    float error_tol = float(2) * max_value * max_value * float(2) * size * std::numeric_limits<float>::epsilon();
    
    for (size_t i = 0; i < size; ++i) {
        for (size_t j = 0; j < size; ++j) {
            float accum = 0;
            for (size_t l = 0; l < size; ++l) {
                accum += A[l*ldabc + i] * B[l*lstride + j*jstride];
            }
            float golden = accum;
            float absdiff = abs(C[j*ldabc+i] - golden);
            if (absdiff > error_tol) {
                return false;
            }
        }
    }
    return true;
}








