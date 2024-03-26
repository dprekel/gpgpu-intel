#include "skl_info.h"
#include "hwinfo.h"

Platform SKL::platform = {
    PRODUCT_FAMILY::IGFX_SKYLAKE,
    PCH_PRODUCT_FAMILY::PCH_UNKNOWN,
    GFX_CORE_FAMILY::IGFX_GEN9_CORE,
    GFX_CORE_FAMILY::IGFX_GEN9_CORE,
    PLATFORM_TYPE::PLATFORM_NONE,
    0,
    0,
    0,
    0,
    GTTYPE::GTTYPE_UNDEFINED
};

FeatureTable SKL::featureTable = {};
void SKL::setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo) {
    FeatureTable* featureTable = hwInfo->featureTable;
    featureTable->flags.ftrGpGpuMidBatchPreempt = true;
    featureTable->flags.ftrGpGpuThreadGroupLevelPreempt = true;
    featureTable->flags.ftrL3IACoherency = true;
    featureTable->flags.ftrGpGpuMidThreadLevelPreempt = true;
    featureTable->flags.ftr3dMidBatchPreempt = true;
    featureTable->flags.ftr3dObjectLevelPreempt = true;
    featureTable->flags.ftrPerCtxtPreemptionGranularityControl = true;
    featureTable->flags.ftrPPGTT = true;
    featureTable->flags.ftrSVM = true;
    featureTable->flags.ftrIA32eGfxPTEs = true;
    featureTable->flags.ftrDisplayYTiling = true;
    featureTable->flags.ftrTranslationTable = true;
    featureTable->flags.ftrUserModeTranslationTable = true;
    featureTable->flags.ftrEnableGuC = true;
    featureTable->flags.ftrFbc = true;
    featureTable->flags.ftrFbc2AddressTranslation = true;
    featureTable->flags.ftrFbcBlitterTracking = true;
    featureTable->flags.ftrFbcCpuTracking = true;
    featureTable->flags.ftrVcs2 = featureTable->flags.ftrGT3 || featureTable->flags.ftrGT4;
    featureTable->flags.ftrVEBOX = true;
    featureTable->flags.ftrSingleVeboxSlice = featureTable->flags.ftrGT1 || featureTable->flags.ftrGT2;
    featureTable->flags.ftrTileY = true;
}

SystemInfo SKL_1x2x6::gtSystemInfo = {0};
void SKL_1x2x6::setupHardwareInfo(const HardwareInfo* hwInfo) {
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
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
    gtSysInfo->MaxEuPerSubSlice = SKL::maxEuPerSubSlice;
    gtSysInfo->MaxSlicesSupported = SKL::maxSlicesSupported;
    gtSysInfo->MaxSubSlicesSupported = SKL::maxSubslicesSupported;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
    setupFeatureAndWorkaroundTable(hwInfo);
}
const HardwareInfo SKL_1x2x6::hwInfo = {
    &SKL::platform,
    &SKL::featureTable,
    &SKL_1x2x6::gtSystemInfo
};

SystemInfo SKL_1x3x6::gtSystemInfo = {0};
void SKL_1x3x6::setupHardwareInfo(const HardwareInfo* hwInfo) {
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->ThreadCount = gtSysInfo->EUCount * SKL::threadsPerEu;
    gtSysInfo->SliceCount = 1;
    gtSysInfo->L3CacheSizeInKb = 768;
    gtSysInfo->L3BankCount = 4;
    gtSysInfo->MaxFillRate = 8;
    gtSysInfo->TotalVsThreads = 336;
    gtSysInfo->TotalHsThreads = 336;
    gtSysInfo->TotalDsThreads = 336;
    gtSysInfo->TotalGsThreads = 336;
    gtSysInfo->TotalPsThreadsWindowerRange = 64;
    gtSysInfo->CsrSizeInMb = 8;
    gtSysInfo->MaxEuPerSubSlice = SKL::maxEuPerSubSlice;
    gtSysInfo->MaxSlicesSupported = SKL::maxSlicesSupported;
    gtSysInfo->MaxSubSlicesSupported = SKL::maxSubslicesSupported;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
    setupFeatureAndWorkaroundTable(hwInfo);
}
const HardwareInfo SKL_1x3x6::hwInfo = {
    &SKL::platform,
    &SKL::featureTable,
    &SKL_1x3x6::gtSystemInfo,
};

SystemInfo SKL_1x3x8::gtSystemInfo = {0};
void SKL_1x3x8::setupHardwareInfo(const HardwareInfo* hwInfo) {
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->ThreadCount = gtSysInfo->EUCount * SKL::threadsPerEu;
    gtSysInfo->SliceCount = 1;
    gtSysInfo->L3CacheSizeInKb = 768;
    gtSysInfo->L3BankCount = 4;
    gtSysInfo->MaxFillRate = 8;
    gtSysInfo->TotalVsThreads = 336;
    gtSysInfo->TotalHsThreads = 336;
    gtSysInfo->TotalDsThreads = 336;
    gtSysInfo->TotalGsThreads = 336;
    gtSysInfo->TotalPsThreadsWindowerRange = 64;
    gtSysInfo->CsrSizeInMb = 8;
    gtSysInfo->MaxEuPerSubSlice = SKL::maxEuPerSubSlice;
    gtSysInfo->MaxSlicesSupported = SKL::maxSlicesSupported;
    gtSysInfo->MaxSubSlicesSupported = SKL::maxSubslicesSupported;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
    setupFeatureAndWorkaroundTable(hwInfo);
}
const HardwareInfo SKL_1x3x8::hwInfo = {
    &SKL::platform,
    &SKL::featureTable,
    &SKL_1x3x8::gtSystemInfo,
};

SystemInfo SKL_2x3x8::gtSystemInfo = {0};
void SKL_2x3x8::setupHardwareInfo(const HardwareInfo* hwInfo) {
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->ThreadCount = gtSysInfo->EUCount * SKL::threadsPerEu;
    gtSysInfo->SliceCount = 2;
    gtSysInfo->L3CacheSizeInKb = 1536;
    gtSysInfo->L3BankCount = 8;
    gtSysInfo->MaxFillRate = 16;
    gtSysInfo->TotalVsThreads = 336;
    gtSysInfo->TotalHsThreads = 336;
    gtSysInfo->TotalDsThreads = 336;
    gtSysInfo->TotalGsThreads = 336;
    gtSysInfo->TotalPsThreadsWindowerRange = 64;
    gtSysInfo->CsrSizeInMb = 8;
    gtSysInfo->MaxEuPerSubSlice = SKL::maxEuPerSubSlice;
    gtSysInfo->MaxSlicesSupported = SKL::maxSlicesSupported;
    gtSysInfo->MaxSubSlicesSupported = SKL::maxSubslicesSupported;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
    setupFeatureAndWorkaroundTable(hwInfo);
}
const HardwareInfo SKL_2x3x8::hwInfo = {
    &SKL::platform,
    &SKL::featureTable,
    &SKL_2x3x8::gtSystemInfo,
};

SystemInfo SKL_3x3x8::gtSystemInfo = {0};
void SKL_3x3x8::setupHardwareInfo(const HardwareInfo* hwInfo) {
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->ThreadCount = gtSysInfo->EUCount * SKL::threadsPerEu;
    gtSysInfo->SliceCount = 3;
    gtSysInfo->L3CacheSizeInKb = 2304;
    gtSysInfo->L3BankCount = 12;
    gtSysInfo->MaxFillRate = 24;
    gtSysInfo->TotalVsThreads = 336;
    gtSysInfo->TotalHsThreads = 336;
    gtSysInfo->TotalDsThreads = 336;
    gtSysInfo->TotalGsThreads = 336;
    gtSysInfo->TotalPsThreadsWindowerRange = 64;
    gtSysInfo->CsrSizeInMb = 8;
    gtSysInfo->MaxEuPerSubSlice = SKL::maxEuPerSubSlice;
    gtSysInfo->MaxSlicesSupported = SKL::maxSlicesSupported;
    gtSysInfo->MaxSubSlicesSupported = SKL::maxSubslicesSupported;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
    setupFeatureAndWorkaroundTable(hwInfo);
}
const HardwareInfo SKL_3x3x8::hwInfo = {
    &SKL::platform,
    &SKL::featureTable,
    &SKL_3x3x8::gtSystemInfo,
};










