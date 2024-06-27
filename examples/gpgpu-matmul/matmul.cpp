#include <gpgpu.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>

#define TILE_SIZE_M     1
#define TILE_GROUP_M    16
#define TILE_SIZE_N     128
#define TILE_GROUP_N    1

int main() {
    int err = 0;
    std::vector<pDevice*> devices = CreateDevices(&err);
    printf("err: %d\n", err);

    std::string build_options = "-DTILE_SIZE_M=" + std::to_string(TILE_SIZE_M)
                              + " -DTILE_GROUP_M=" + std::to_string(TILE_GROUP_M)
                              + " -DTILE_SIZE_N=" + std::to_string(TILE_SIZE_N)
                              + " -DTILE_GROUP_N=" + std::to_string(TILE_GROUP_N);
    pContext* context = CreateContext(devices[0], &err);
    printf("Context creation: %d\n", err);
    pKernel* kernel = BuildKernel(context, "matmul.cl", build_options.c_str(), 0, true, &err);
    printf("Kernel build: %d\n", err);

    size_t size = 3968;
    size_t matrix_memory_size = size*size*sizeof(float);
    
    pBuffer* matrix_A = CreateBuffer(context, matrix_memory_size, &err);
    pBuffer* matrix_B = CreateBuffer(context, matrix_memory_size, &err);
    pBuffer* matrix_C = CreateBuffer(context, matrix_memory_size, &err);
    printf("Buffer ptr: %p\n", matrix_A);

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
    err = EnqueueNDRangeKernel(context, kernel, 2, global, local);
    printf("err: %d\n", err);

    err = ReleaseKernel(kernel);
    err = ReleaseContext(context);
    err = ReleaseDevice(devices[0]);
    err = ReleaseBuffer(matrix_A);
    err = ReleaseBuffer(matrix_B);
    err = ReleaseBuffer(matrix_C);
    return 0;
}


