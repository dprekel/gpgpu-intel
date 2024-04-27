#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <gpgpu.h>

#define TILE_SIZE_M     1
#define TILE_GROUP_M    16
#define TILE_SIZE_N     128
#define TILE_GROUP_N    1

int main() {
    GPU* gpuInfo = (GPU*)malloc(sizeof(GPU));
    int err = CreateDevice(gpuInfo);
    GetInfo(gpuInfo);

    const char* filename = "matmul.cl";
    std::string build_options = "-DTILE_SIZE_M=" + std::to_string(TILE_SIZE_M)
                              + " -DTILE_GROUP_M=" + std::to_string(TILE_GROUP_M)
                              + " -DTILE_SIZE_N=" + std::to_string(TILE_SIZE_N)
                              + " -DTILE_GROUP_N=" + std::to_string(TILE_GROUP_N);
    err = BuildKernel(gpuInfo, filename, build_options.c_str());
    printf("Kernel build: %d\n", err);
    err = CreateContext(gpuInfo);
    printf("Context creation: %d\n", err);
    //void* ptrToBuffer = CreateBuffer(gpuInfo, 4096);
    //printf("Buffer ptr: %p\n", ptrToBuffer);

    size_t size = 3968;
    // number of work items per work group dimension
    const size_t local[2] = {TILE_GROUP_M, TILE_GROUP_N};
    // total number of work items in each dimension
    const size_t global[2] = {size/TILE_SIZE_M, size/TILE_SIZE_N};
    err = EnqueueNDRangeKernel(gpuInfo, 2, NULL, global, local);
    printf("err: %d\n", err);

    err = ReleaseObjects(gpuInfo);
    return 0;
}
