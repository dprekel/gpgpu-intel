#include "hwinfo.h"

const struct Platform SKL::platform = {
    IGFX_SKYLAKE,
    PCH_UNKNOWN,
    IGFX_GEN9_CORE,
    IGFX_GEN9_CORE,
    PLATFORM_NONE,
    0,
    0,
    0,
    0,
    GTTYPE_UNDEFINED
};

struct SystemInfo SKL_1x2x6::gtSystemInfo = {0};

void SKL_1x2x6::setupHardwareInfo(HardwareInfo* hwInfo) {
    SystemInfo* gtSysInfo = &hwInfo->gtSystemInfo;
    gtSysInfo->ThreadCount = gtSysInfo->EUCount * SKL::threadsPerEu;
    gtSysInfo->SliceCount = 1;
    gtSysInfo->L3CacheSizeInKb = 384;
    gtSysInfo->L3BankCount = 2;
    gtSysInfo->MaxFillRate = 8;
    gtSysInfo->TotalVsThreads = 336;
    gtSysInfo->TotalHsThreads = 336;
    gtSysInfo->TotalDsThreads = 336;
    gtSysInfo->TotalGsThreads = 336;
    gtSysInfo->TotalPsThreadsWindowerRange = 64;
    gtSysInfo->CsrSizeInMb = 8;
    gtSysInfo->MaxEuPerSubSlice = SKL::maxEuPerSubslice;
    gtSysInfo->MaxSlicesSupported = SKL::maxSlicesSupported;
    gtSysInfo->MaxSubSlicesSupported = SKL::maxSubslicesSupported;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
}

const struct HardwareInfo SKL_1x2x6::hwInfo = {
    &SKL::platform,
    &SKL::featureTable,
    &SKL_1x2x6::gtSystemInfo
};



