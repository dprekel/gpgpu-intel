#include <stdio.h>
#include <stdint.h>

#include "gpgpu.h"
#include "driver.h"
#include "log.h"

void logGPUInfo(struct gpuInfo* gpuInfo) {
    printf("File descriptor: %d\n", gpuInfo->fd);
    printf("Driver version: %s\n", gpuInfo->driver_name);
    printf("Chipset ID: %d\n", gpuInfo->chipset_id);
    printf("Revision ID: %d\n", gpuInfo->revision_id);
    printf("MaxSliceCount: %hu\n", gpuInfo->maxSliceCount);
    printf("MaxSubSliceCount: %hu\n", gpuInfo->maxSubSliceCount);
    printf("MaxEUCount: %hu\n", gpuInfo->maxEUCount);
    printf("\n");
    printf("Available Engines                         Capabilities\n");
    printf("---------------------------------------------------------------------\n");
    auto engineData = reinterpret_cast<drm_i915_query_engine_info*>(gpuInfo->engines);
    for (uint32_t i = 0; i < engineData->num_engines; i++) {
        switch (engineData->engines[i].engine.engine_class) {
            case I915_ENGINE_CLASS_RENDER:
                printf("RCS  (Render Command Streamer)            %s\n", decodeCapabilities(engineData->engines[i].capabilities));
                break;
            case I915_ENGINE_CLASS_COPY:
                printf("BCS  (Blitter Command Streamer)           %s\n", decodeCapabilities(engineData->engines[i].capabilities));
                break;
            case I915_ENGINE_CLASS_VIDEO:
                printf("VCS  (Video Command Streamer)             %s\n", decodeCapabilities(engineData->engines[i].capabilities));
                break;
            case I915_ENGINE_CLASS_VIDEO_ENHANCE:
                printf("VECS (Video Enhance Command Streamer)     %s\n", decodeCapabilities(engineData->engines[i].capabilities));
                break;
            default:
                break;
        }
    }
    printf("\n");
    printf("DrmVmId: %d\n", gpuInfo->drmVmId);
    printf("GTT size: %lu\n", gpuInfo->gttSize);
}


const char* decodeCapabilities(uint64_t capability) {
    const char* cap;
    switch (capability) {
        case I915_CAPABILITY_NOT_SPECIFIED:
            cap = "Not specified";
            break;
        case I915_VIDEO_CLASS_CAPABILITY_HEVC:
            cap = "HEVC support";
            break;
        case I915_VIDEO_AND_ENHANCE_CLASS_CAPABILITY_SFC:
            cap = "SFC support";
            break;
        default:
            cap = "Unknown capability";
            break;
    }
    return cap;
}




