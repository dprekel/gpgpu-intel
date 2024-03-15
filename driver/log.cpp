#include <stdio.h>
#include <stdint.h>

#include "gpgpu.h"
#include "gpuinit.h"
#include "drm_structs.h"
#include "log.h"

void logGPUInfo(GPU* gpuInfo) {
    printf("File descriptor: %d\n", gpuInfo->fd);
    printf("Driver version: %s\n", gpuInfo->driver_name);
    printf("Chipset ID: %d\n", gpuInfo->chipset_id);
    printf("Revision ID: %d\n", gpuInfo->revision_id);
    auto descriptor = static_cast<DeviceDescriptor*>(gpuInfo->descriptor);
    printf("Device Name: %s\n", descriptor->devName);
    showTopology(gpuInfo);
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
    printf("Context Information\n");
    printf("---------------------------------------------------------------------\n");
    printf("Selected Engine:\n");
    printf("DrmVmId: %d\n", gpuInfo->drmVmId);
    printf("GTT size: %lu\n", gpuInfo->gttSize);
    if (gpuInfo->nonPersistentContextsSupported) {
        printf("Persistence Support: No\n");
    }
    else if (!gpuInfo->nonPersistentContextsSupported) {
        printf("Persistence Support: Yes\n");
    }
    if (gpuInfo->preemptionSupported) {
        printf("Preemption Support: Yes\n");
    }
    else if (!gpuInfo->preemptionSupported) {
        printf("Preemption Support: No\n");
    }
}

void showTopology(GPU* gpuInfo) {
    uint16_t sliceCount = gpuInfo->sliceCount;
    uint16_t subSliceCount = gpuInfo->subSliceCount;
    uint16_t euCount = gpuInfo->euCount;
    uint16_t subSliceCountPerSlice = gpuInfo->subSliceCountPerSlice;
    uint16_t euCountPerSubSlice = gpuInfo->euCountPerSubSlice;

    printf("\n");
    printf("Execution Units\n");
    printf("---------------------------------------------------------------------\n");
    printf("Slices: %hu     Subslices: %hu      EUs: %hu\n", sliceCount, subSliceCount, euCount);
    printf("Subslices per Slice: %hu\n", subSliceCountPerSlice);
    printf("EUs per Subclice: %hu\n", euCountPerSubSlice);
    for (uint16_t i = 0; i < sliceCount; i++) {
        printf(" ");
        for (uint16_t j = 0; j < subSliceCountPerSlice; j++) {
            if (j == subSliceCountPerSlice - 1) {
                printf("___________\n");
            }
            else {
                printf("_______________");
            }
        }
        for (uint16_t j = 0; j < subSliceCountPerSlice; j++) {
            if (j == subSliceCountPerSlice -1) {
                printf("|EU EU EU EU|\n");
            }
            else {
                printf("|EU EU EU EU|  ");
            }
        }
        for (uint16_t j = 0; j < subSliceCountPerSlice; j++) {
            if (j == subSliceCountPerSlice - 1) {
                printf("|           |\n");
            }
            else {
                printf("|           |  ");
            }
        }
        for (uint16_t j = 0; j < subSliceCountPerSlice; j++) {
            if (j == subSliceCountPerSlice -1) {
                printf("|EU EU EU EU|\n");
            }
            else {
                printf("|EU EU EU EU|  ");
            }
        }
        printf(" ");
        for (uint16_t j = 0; j < subSliceCountPerSlice; j++) {
            if (j == subSliceCountPerSlice - 1) {
                for (int k = 0; k < 11; k++) {printf("\u203E"); }
                printf("\n");
            }
            else {
                for (int k = 0; k < 15; k++) {printf("\u203E"); }
            }
        }
    }
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




