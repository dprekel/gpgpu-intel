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
    int err;
    const char* filename;

    GPU* gpuInfo = (GPU*)malloc(sizeof(GPU));
    err = gpInitGPU(gpuInfo);
    logGPUInfo(gpuInfo);

    filename = "matmul.cl";
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
    err = EnqueueNDRangeKernel(gpuInfo);

    return 0;
}
