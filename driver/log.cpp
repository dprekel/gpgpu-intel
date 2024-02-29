#include <stdio.h>
#include <stdint.h>

#include "gpgpu.h"
#include "driver.h"

void logGPUInfo(struct gpuInfo* gpuInfo) {
    printf("File descriptor: %d\n", gpuInfo->fd);
    printf("Driver version: %s\n", gpuInfo->driver_name);
    printf("Chipset ID: %d\n", gpuInfo->chipset_id);
    printf("Revision ID: %d\n", gpuInfo->revision_id);
    printf("MaxSliceCount: %hu\n", gpuInfo->maxSliceCount);
    printf("MaxSubSliceCount: %hu\n", gpuInfo->maxSubSliceCount);
    printf("MaxEUCount: %hu\n", gpuInfo->maxEUCount);
    printf("DrmVmId: %d\n", gpuInfo->drmVmId);
    printf("GTT size: %lu\n", gpuInfo->gttSize);
}
