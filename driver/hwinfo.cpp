#include "kbl_info.h"
#include "hwinfo.h"


// Meteorlake
// Lunarlake
// Alchemist
// Battlemage

// Tigerlake (GEN12LP)
Platform TGLLP::platform = {
    PRODUCT_FAMILY::IGFX_TIGERLAKE_LP,
    PCH_PRODUCT_FAMILY::PCH_UNKNOWN,
    GFX_CORE_FAMILY::IGFX_GEN12LP_CORE,
    GFX_CORE_FAMILY::IGFX_GEN12LP_CORE,
    PLATFORM_TYPE::PLATFORM_NONE,
    0,
    0,
    0,
    0,
    GTTYPE::GTTYPE_UNDEFINED
};
FeatureTable TGLLP::featureTable = {0};
void TGLLP::setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo) {
    FeatureTable* featureTable = hwInfo->featureTable;
    featureTable->flags.ftrL3IACoherency = true;
    featureTable->flags.ftrPPGTT = true;
    featureTable->flags.ftrSVM = true;
    featureTable->flags.ftrIA32eGfxPTEs = true;
    featureTable->flags.ftrStandardMipTailFormat = true;
    featureTable->flags.ftrTranslationTable = true;
    featureTable->flags.ftrUserModeTranslationTable = true;
    featureTable->flags.ftrTileMappedResource = true;
    featureTable->flags.ftrEnableGuC = true;
    featureTable->flags.ftrFbc = true;
    featureTable->flags.ftrFbc2AddressTranslation = true;
    featureTable->flags.ftrFbcBlitterTracking = true;
    featureTable->flags.ftrFbcCpuTracking = true;
    featureTable->flags.ftrTileY = true;
    featureTable->flags.ftrAstcHdr2D = true;
    featureTable->flags.ftrAstcLdr2D = true;
    featureTable->flags.ftr3dMidBatchPreempt = true;
    featureTable->flags.ftrGpGpuMidBatchPreempt = true;
    featureTable->flags.ftrGpGpuThreadGroupLevelPreempt = true;
    featureTable->flags.ftrPerCtxtPreemptionGranularityControl = true;
}
SystemInfo TGLLP_1x6x16::gtSystemInfo = {0};
void TGLLP_1x6x16::setupHardwareInfo(const HardwareInfo* hwInfo) {
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->ThreadCount = gtSysInfo->EUCount * TGLLP::threadsPerEu;
    gtSysInfo->SliceCount = 1;
    gtSysInfo->DualSubSliceCount = 6;
    gtSysInfo->L3CacheSizeInKb = 3840;
    gtSysInfo->L3BankCount = 8;
    gtSysInfo->MaxFillRate = 16;
    gtSysInfo->TotalVsThreads = 336;
    gtSysInfo->TotalHsThreads = 336;
    gtSysInfo->TotalDsThreads = 336;
    gtSysInfo->TotalGsThreads = 336;
    gtSysInfo->TotalPsThreadsWindowerRange = 64;
    gtSysInfo->CsrSizeInMb = 8;
    gtSysInfo->MaxEuPerSubSlice = TGLLP::maxEuPerSubSlice;
    gtSysInfo->MaxSlicesSupported = TGLLP::maxSlicesSupported;
    gtSysInfo->MaxSubSlicesSupported = TGLLP::maxSubslicesSupported;
    gtSysInfo->MaxDualSubSlicesSupported = TGLLP::maxDualSubSlicesSupported;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
    gtSysInfo->CCSInfo.IsValid = true;
    gtSysInfo->CCSInfo.NumberOfCCSEnabled = 1;
    gtSysInfo->CCSInfo.Instances.CCSEnableMask = 0b1;
    setupFeatureAndWorkaroundTable(hwInfo);
}
const HardwareInfo TGLLP_1x6x16::hwInfo = {
    &TGLLP::platform,
    &TGLLP::featureTable,
    &TGLLP_1x1x16::gtSystemInfo
};
SystemInfo TGLLP_1x2x16::gtSystemInfo = {0};
void TGLLP_1x2x16::setupHardwareInfo(const HardwareInfo* hwInfo) {
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->ThreadCount = gtSysInfo->EUCount * TGLLP::threadsPerEu;
    gtSysInfo->SliceCount = 1;
    gtSysInfo->DualSubSliceCount = 2;
    gtSysInfo->L3CacheSizeInKb = 1920;
    gtSysInfo->L3BankCount = 4;
    gtSysInfo->MaxFillRate = 16;
    gtSysInfo->TotalVsThreads = 224;
    gtSysInfo->TotalHsThreads = 224;
    gtSysInfo->TotalDsThreads = 224;
    gtSysInfo->TotalGsThreads = 224;
    gtSysInfo->TotalPsThreadsWindowerRange = 64;
    gtSysInfo->CsrSizeInMb = 8;
    gtSysInfo->MaxEuPerSubSlice = TGLLP::maxEuPerSubSlice;
    gtSysInfo->MaxSlicesSupported = TGLLP::maxSlicesSupported;
    gtSysInfo->MaxSubSlicesSupported = TGLLP::maxSubslicesSupported;
    gtSysInfo->MaxDualSubSlicesSupported = TGLLP::maxDualSubSlicesSupported;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
    gtSysInfo->CCSInfo.IsValid = true;
    gtSysInfo->CCSInfo.NumberOfCCSEnabled = 1;
    gtSysInfo->CCSInfo.Instances.CCSEnableMask = 0b1;
    setupFeatureAndWorkaroundTable(hwInfo);
}
const HardwareInfo TGLLP_1x2x16::hwInfo = {
    &TGLLP::platform,
    &TGLLP::featureTable,
    &TGLLP_1x2x16::gtSystemInfo
};




// DG1 (GEN12LP)
Platform DG1::platform = {
    PRODUCT_FAMILY::IGFX_DG1,
    PCH_PRODUCT_FAMILY::PCH_UNKNOWN,
    GFX_CORE_FAMILY::IGFX_GEN12LP_CORE,
    GFX_CORE_FAMILY::IGFX_GEN12LP_CORE,
    PLATFORM_TYPE::PLATFORM_NONE,
    0,
    0,
    0,
    0,
    GTTYPE::GTTYPE_UNDEFINED
};
FeatureTable DG1::featureTable = {0};
void DG1::setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo) {
    FeatureTable* featureTable = hwInfo->featureTable;
    featureTable->flags.ftrL3IACoherency = true;
    featureTable->flags.ftrPPGTT = true;
    featureTable->flags.ftrSVM = true;
    featureTable->flags.ftrIA32eGfxPTEs = true;
    featureTable->flags.ftrStandardMipTailFormat = true;
    featureTable->flags.ftrLocalMemory = true;
    featureTable->flags.ftrTranslationTable = true;
    featureTable->flags.ftrUserModeTranslationTable = true;
    featureTable->flags.ftrTileMappedResource = true;
    featureTable->flags.ftrEnableGuC = true;
    featureTable->flags.ftrFbc = true;
    featureTable->flags.ftrFbc2AddressTranslation = true;
    featureTable->flags.ftrFbcBlitterTracking = true;
    featureTable->flags.ftrFbcCpuTracking = true;
    featureTable->flags.ftrTileY = true;
    featureTable->flags.ftrAstcHdr2D = true;
    featureTable->flags.ftrAstcLdr2D = true;
    featureTable->flags.ftr3dMidBatchPreempt = true;
    featureTable->flags.ftrGpGpuMidBatchPreempt = true;
    featureTable->flags.ftrGpGpuThreadGroupLevelPreempt = true;
    featureTable->flags.ftrPerCtxtPreemptionGranularityControl = true;
    featureTable->flags.ftrBcsInfo = maxNBitValue(1);
}




// Rocketlake (GEN12)
Platform RKL::platform = {
    PRODUCT_FAMILY::IGFX_ROCKETLAKE,
    PCH_PRODUCT_FAMILY::PCH_UNKNOWN,
    GFX_CORE_FAMILY::IGFX_GEN12LP_CORE,
    GFX_CORE_FAMILY::IGFX_GEN12LP_CORE,
    PLATFORM_TYPE::PLATFORM_NONE,
    0,
    0,
    0,
    0,
    GTTYPE::GTTYPE_UNDEFINED
};
FeatureTable RKL::featureTable = {0};
void RKL::setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo) {
    FeatureTable* featureTable = hwInfo->featureTable;
    featureTable->flags.ftrL3IACoherency = true;
    featureTable->flags.ftrPPGTT = true;
    featureTable->flags.ftrSVM = true;
    featureTable->flags.ftrIA32eGfxPTEs = true;
    featureTable->flags.ftrStandardMipTailFormat = true;
    featureTable->flags.ftrTranslationTable = true;
    featureTable->flags.ftrUserModeTranslationTable = true;
    featureTable->flags.ftrTileMappedResource = true;
    featureTable->flags.ftrEnableGuC = true;
    featureTable->flags.ftrFbc = true;
    featureTable->flags.ftrFbc2AddressTranslation = true;
    featureTable->flags.ftrFbcBlitterTracking = true;
    featureTable->flags.ftrFbcCpuTracking = true;
    featureTable->flags.ftrTileY = true;
    featureTable->flags.ftrAstcHdr2D = true;
    featureTable->flags.ftrAstcLdr2D = true;
    featureTable->flags.ftr3dMidBatchPreempt = true;
    featureTable->flags.ftrGpGpuMidBatchPreempt = true;
    featureTable->flags.ftrGpGpuThreadGroupLevelPreempt = true;
    featureTable->flags.ftrPerCtxtPreemptionGranularityControl = true;
}
SystemInfo RKL_HW_CONFIG::gtSystemInfo = {0};
void RKL_HW_CONFIG::setupHardwareInfo(const HardwareInfo* hwInfo) {
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->ThreadCount = gtSysInfo->EUCount * RKL::threadsPerEu;
    gtSysInfo->DualSubSliceCount = gtSysInfo->SubSliceCount;
    gtSysInfo->L3CacheSizeInKb = 1920;
    gtSysInfo->L3BankCount = 4;
    gtSysInfo->MaxFillRate = 8;
    gtSysInfo->TotalVsThreads = 0;
    gtSysInfo->TotalHsThreads = 0;
    gtSysInfo->TotalDsThreads = 0;
    gtSysInfo->TotalGsThreads = 0;
    gtSysInfo->TotalPsThreadsWindowerRange = 64;
    gtSysInfo->CsrSizeInMb = 8;
    gtSysInfo->MaxEuPerSubSlice = RKL::maxEuPerSubSlice;
    gtSysInfo->MaxSlicesSupported = RKL::maxSlicesSupported;
    gtSysInfo->MaxSubSlicesSupported = 2;
    gtSysInfo->MaxDualSubSlicesSupported = 2;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
    gtSysInfo->CCSInfo.IsValid = true;
    gtSysInfo->CCSInfo.NumberOfCCSEnabled = 1;
    gtSysInfo->CCSInfo.Instances.CCSEnableMask = 0b1;
    setupFeatureAndWorkaroundTable(hwInfo);
}
const HardwareInfo RKL_HW_CONFIG::hwInfo = {
    &RKL::platform,
    &RKL::featureTable,
    &RKL_HW_CONFIG::gtSystemInfo
};




// Alderlake_S (GEN12LP)
Platform ADLS::platform = {
    PRODUCT_FAMILY::IGFX_ALDERLAKE_S,
    PCH_PRODUCT_FAMILY::PCH_UNKNOWN,
    GFX_CORE_FAMILY::IGFX_GEN12LP_CORE,
    GFX_CORE_FAMILY::IGFX_GEN12LP_CORE,
    PLATFORM_TYPE::PLATFORM_NONE,
    0,
    0,
    0,
    0,
    GTTYPE::GTTYPE_UNDEFINED
};
FeatureTable ADLS::featureTable = {0};
void ADLS::setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo) {
    FeatureTable* featureTable = hwInfo->featureTable;
    featureTable->flags.ftrL3IACoherency = true;
    featureTable->flags.ftrPPGTT = true;
    featureTable->flags.ftrSVM = true;
    featureTable->flags.ftrIA32eGfxPTEs = true;
    featureTable->flags.ftrStandardMipTailFormat = true;
    featureTable->flags.ftrTranslationTable = true;
    featureTable->flags.ftrUserModeTranslationTable = true;
    featureTable->flags.ftrTileMappedResource = true;
    featureTable->flags.ftrEnableGuC = true;
    featureTable->flags.ftrFbc = true;
    featureTable->flags.ftrFbc2AddressTranslation = true;
    featureTable->flags.ftrFbcBlitterTracking = true;
    featureTable->flags.ftrFbcCpuTracking = true;
    featureTable->flags.ftrTileY = true;
    featureTable->flags.ftrAstcHdr2D = true;
    featureTable->flags.ftrAstcLdr2D = true;
    featureTable->flags.ftr3dMidBatchPreempt = true;
    featureTable->flags.ftrGpGpuMidBatchPreempt = true;
    featureTable->flags.ftrGpGpuThreadGroupLevelPreempt = true;
    featureTable->flags.ftrPerCtxtPreemptionGranularityControl = true;
}
SystemInfo ADLS_HW_CONFIG::gtSystemInfo = {0};
void ADLS_HW_CONFIG::setupHardwareInfo(const HardwareInfo* hwInfo) {
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->ThreadCount = gtSysInfo->EUCount * ADLS::threadsPerEu;
    gtSysInfo->DualSubSliceCount = gtSysInfo->SubSliceCount;
    gtSysInfo->L3CacheSizeInKb = 1920;
    gtSysInfo->L3BankCount = 4;
    gtSysInfo->MaxFillRate = 8;
    gtSysInfo->TotalVsThreads = 0;
    gtSysInfo->TotalHsThreads = 0;
    gtSysInfo->TotalDsThreads = 0;
    gtSysInfo->TotalGsThreads = 0;
    gtSysInfo->TotalPsThreadsWindowerRange = 64;
    gtSysInfo->CsrSizeInMb = 8;
    gtSysInfo->MaxEuPerSubSlice = RKL::maxEuPerSubSlice;
    gtSysInfo->MaxSlicesSupported = RKL::maxSlicesSupported;
    gtSysInfo->MaxSubSlicesSupported = 1;
    gtSysInfo->MaxDualSubSlicesSupported = 2;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
    gtSysInfo->CCSInfo.IsValid = true;
    gtSysInfo->CCSInfo.NumberOfCCSEnabled = 1;
    gtSysInfo->CCSInfo.Instances.CCSEnableMask = 0b1;
    setupFeatureAndWorkaroundTable(hwInfo);
}
const HardwareInfo ADLS_HW_CONFIG::hwInfo = {
    &ADLS::platform,
    &ADLS::featureTable,
    &ADLS_HW_CONFIG::gtSystemInfo
};




// Icelake_LP (GEN11)
Platform ICLLP::platform = {
    PRODUCT_FAMILY::IGFX_ICELAKE_LP,
    PCH_PRODUCT_FAMILY::PCH_UNKNOWN,
    GFX_CORE_FAMILY::IGFX_GEN11_CORE,
    GFX_CORE_FAMILY::IGFX_GEN11_CORE,
    PLATFORM_TYPE::PLATFORM_NONE,
    0,
    0,
    0,
    0,
    GTTYPE::GTTYPE_UNDEFINED
};
FeatureTable ICLLP::featureTable = {0};
void ICLLP::setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo) {
    FeatureTable* featureTable = hwInfo->featureTable;
    featureTable->flags.ftrL3IACoherency = true;
    featureTable->flags.ftrPPGTT = true;
    featureTable->flags.ftrSVM = true;
    featureTable->flags.ftrIA32eGfxPTEs = true;
    featureTable->flags.ftrStandardMipTailFormat = true;
    featureTable->flags.ftrDisplayYTiling = true;
    featureTable->flags.ftrTranslationTable = true;
    featureTable->flags.ftrUserModeTranslationTable = true;
    featureTable->flags.ftrTileMappedResource = true;
    featureTable->flags.ftrEnableGuC = true;
    featureTable->flags.ftrFbc = true;
    featureTable->flags.ftrFbc2AddressTranslation = true;
    featureTable->flags.ftrFbcBlitterTracking = true;
    featureTable->flags.ftrFbcCpuTracking = true;
    featureTable->flags.ftrTileY = true;
    featureTable->flags.ftrAstcHdr2D = true;
    featureTable->flags.ftrAstcLdr2D = true;
    featureTable->flags.ftr3dMidBatchPreempt = true;
    featureTable->flags.ftrGpGpuMidBatchPreempt = true;
    featureTable->flags.ftrGpGpuMidThreadLevelPreempt = true;
    featureTable->flags.ftrGpGpuThreadGroupLevelPreempt = true;
    featureTable->flags.ftrPerCtxtPreemptionGranularityControl = true;
}
SystemInfo ICLLP_1x4x8::gtSystemInfo = {0};
void ICLLP_1x4x8::setupHardwareInfo(const HardwareInfo* hwInfo) {
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->ThreadCount = gtSysInfo->EUCount * ICLLP::threadsPerEu;
    gtSysInfo->SliceCount = 1;
    gtSysInfo->L3CacheSizeInKb = 2304;
    gtSysInfo->L3BankCount = 6;
    gtSysInfo->MaxFillRate = 8;
    gtSysInfo->TotalVsThreads = 364;
    gtSysInfo->TotalHsThreads = 224;
    gtSysInfo->TotalDsThreads = 364;
    gtSysInfo->TotalGsThreads = 224;
    gtSysInfo->TotalPsThreadsWindowerRange = 128;
    gtSysInfo->CsrSizeInMb = 5;
    gtSysInfo->MaxEuPerSubSlice = ICLLP::maxEuPerSubSlice;
    gtSysInfo->MaxSlicesSupported = ICLLP::maxSlicesSupported;
    gtSysInfo->MaxSubSlicesSupported = ICLLP::maxSubslicesSupported;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
    setupFeatureAndWorkaroundTable(hwInfo);
}
const HardwareInfo ICLLP_1x4x8::hwInfo = {
    &ICLLP::platform,
    &ICLLP::featureTable,
    &ICLLP_1x4x8::gtSystemInfo
};
SystemInfo ICLLP_1x6x8::gtSystemInfo = {0};
void ICLLP_1x6x8::setupHardwareInfo(const HardwareInfo* hwInfo) {
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->ThreadCount = gtSysInfo->EUCount * ICLLP::threadsPerEu;
    gtSysInfo->SliceCount = 1;
    gtSysInfo->L3CacheSizeInKb = 2304;
    gtSysInfo->L3BankCount = 6;
    gtSysInfo->MaxFillRate = 8;
    gtSysInfo->TotalVsThreads = 364;
    gtSysInfo->TotalHsThreads = 224;
    gtSysInfo->TotalDsThreads = 364;
    gtSysInfo->TotalGsThreads = 224;
    gtSysInfo->TotalPsThreadsWindowerRange = 128;
    gtSysInfo->CsrSizeInMb = 5;
    gtSysInfo->MaxEuPerSubSlice = ICLLP::maxEuPerSubSlice;
    gtSysInfo->MaxSlicesSupported = ICLLP::maxSlicesSupported;
    gtSysInfo->MaxSubSlicesSupported = ICLLP::maxSubslicesSupported;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
    setupFeatureAndWorkaroundTable(hwInfo);
}
const HardwareInfo ICLLP_1x6x8::hwInfo = {
    &ICLLP::platform,
    &ICLLP::featureTable,
    &ICLLP_1x6x8::gtSystemInfo
};
SystemInfo ICLLP_1x8x8::gtSystemInfo = {0};
void ICLLP_1x8x8::setupHardwareInfo(const HardwareInfo* hwInfo) {
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->ThreadCount = gtSysInfo->EUCount * ICLLP::threadsPerEu;
    gtSysInfo->SliceCount = 1;
    gtSysInfo->L3CacheSizeInKb = 3072;
    gtSysInfo->L3BankCount = 8;
    gtSysInfo->MaxFillRate = 16;
    gtSysInfo->TotalVsThreads = 336;
    gtSysInfo->TotalHsThreads = 336;
    gtSysInfo->TotalDsThreads = 336;
    gtSysInfo->TotalGsThreads = 336;
    gtSysInfo->TotalPsThreadsWindowerRange = 64;
    gtSysInfo->CsrSizeInMb = 5;
    gtSysInfo->MaxEuPerSubSlice = ICLLP::maxEuPerSubSlice;
    gtSysInfo->MaxSlicesSupported = ICLLP::maxSlicesSupported;
    gtSysInfo->MaxSubSlicesSupported = ICLLP::maxSubslicesSupported;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
    setupFeatureAndWorkaroundTable(hwInfo);
}
const HardwareInfo ICLLP_1x8x8::hwInfo = {
    &ICLLP::platform,
    &ICLLP::featureTable,
    &ICLLP_1x8x8::gtSystemInfo
};





// Lakefield (GEN11)
Platform LKF::platform = {
    PRODUCT_FAMILY::IGFX_LAKEFIELD,
    PCH_PRODUCT_FAMILY::PCH_UNKNOWN,
    GFX_CORE_FAMILY::IGFX_GEN11_CORE,
    GFX_CORE_FAMILY::IGFX_GEN11_CORE,
    PLATFORM_TYPE::PLATFORM_NONE,
    0,
    0,
    0,
    0,
    GTTYPE::GTTYPE_UNDEFINED
};
FeatureTable LKF::featureTable = {0};
void LKF::setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo) {
    FeatureTable* featureTable = hwInfo->featureTable;
    featureTable->flags.ftrL3IACoherency = true;
    featureTable->flags.ftrPPGTT = true;
    featureTable->flags.ftrSVM = true;
    featureTable->flags.ftrIA32eGfxPTEs = true;
    featureTable->flags.ftrStandardMipTailFormat = true;
    featureTable->flags.ftrDisplayYTiling = true;
    featureTable->flags.ftrTranslationTable = true;
    featureTable->flags.ftrUserModeTranslationTable = true;
    featureTable->flags.ftrTileMappedResource = true;
    featureTable->flags.ftrEnableGuC = true;
    featureTable->flags.ftrFbc = true;
    featureTable->flags.ftrFbc2AddressTranslation = true;
    featureTable->flags.ftrFbcBlitterTracking = true;
    featureTable->flags.ftrFbcCpuTracking = true;
    featureTable->flags.ftrTileY = true;
    featureTable->flags.ftrAstcHdr2D = true;
    featureTable->flags.ftrAstcLdr2D = true;
    featureTable->flags.ftr3dMidBatchPreempt = true;
    featureTable->flags.ftrGpGpuMidBatchPreempt = true;
    featureTable->flags.ftrGpGpuMidThreadLevelPreempt = true;
    featureTable->flags.ftrGpGpuThreadGroupLevelPreempt = true;
    featureTable->flags.ftrPerCtxtPreemptionGranularityControl = true;
}




// Elkhart Lake (GEN11)
Platform EHL::platform = {
    PRODUCT_FAMILY::IGFX_ELKHARTLAKE,
    PCH_PRODUCT_FAMILY::PCH_UNKNOWN,
    GFX_CORE_FAMILY::IGFX_GEN11_CORE,
    GFX_CORE_FAMILY::IGFX_GEN11_CORE,
    PLATFORM_TYPE::PLATFORM_NONE,
    0,
    0,
    0,
    0,
    GTTYPE::GTTYPE_UNDEFINED
};
FeatureTable EHL::featureTable = {0};
void EHL::setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo) {
    FeatureTable* featureTable = hwInfo->featureTable;
    featureTable->flags.ftrL3IACoherency = true;
    featureTable->flags.ftrPPGTT = true;
    featureTable->flags.ftrSVM = true;
    featureTable->flags.ftrIA32eGfxPTEs = true;
    featureTable->flags.ftrStandardMipTailFormat = true;
    featureTable->flags.ftrDisplayYTiling = true;
    featureTable->flags.ftrTranslationTable = true;
    featureTable->flags.ftrUserModeTranslationTable = true;
    featureTable->flags.ftrTileMappedResource = true;
    featureTable->flags.ftrEnableGuC = true;
    featureTable->flags.ftrFbc = true;
    featureTable->flags.ftrFbc2AddressTranslation = true;
    featureTable->flags.ftrFbcBlitterTracking = true;
    featureTable->flags.ftrFbcCpuTracking = true;
    featureTable->flags.ftrTileY = true;
    featureTable->flags.ftrAstcHdr2D = true;
    featureTable->flags.ftrAstcLdr2D = true;
    featureTable->flags.ftr3dMidBatchPreempt = true;
    featureTable->flags.ftrGpGpuMidBatchPreempt = true;
    featureTable->flags.ftrGpGpuMidThreadLevelPreempt = true;
    featureTable->flags.ftrGpGpuThreadGroupLevelPreempt = true;
    featureTable->flags.ftrPerCtxtPreemptionGranularityControl = true;
}




// Skylake (GEN9)
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
FeatureTable SKL::featureTable = {0};
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



// Kabylake (GEN9)
Platform KBL::platform = {
    PRODUCT_FAMILY::IGFX_KABYLAKE,
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
FeatureTable KBL::featureTable = {0};
void KBL::setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo) {
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
    featureTable->flags.ftrTileY = true;
}




// Coffeelake (GEN9)
Platform CFL::platform = {
    PRODUCT_FAMILY::IGFX_COFFEELAKE,
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
FeatureTable CFL::feautureTable = {0};
void CFL::setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo) {
    FeatureTable* featureTable = hwInfo->featureTable;
    featureTable->flags.ftrGpGpuMidBatchPreempt = true;
    featureTable->flags.ftrGpGpuThreadGroupLevelPreempt = true;
    featureTable->flags.ftrL3IACoherency = true;
    featureTable->flags.ftrVEBOX = true;
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
    featureTable->flags.ftrTileY = true;
}




// Gemini Lake (GEN9)
Platform GLK::platform = {
    PRODUCT_FAMILY::IGFX_GEMINILAKE,
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
FeatureTable GLK::featureTable = {0};
void GLK::setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo) {
    FeatureTable* featureTable = hwInfo->featureTable;
    featureTable->flags.ftrGpGpuMidBatchPreempt = true;
    featureTable->flags.ftrGpGpuThreadGroupLevelPreempt = true;
    featureTable->flags.ftrL3IACoherency = true;
    featureTable->flags.ftrGpGpuMidThreadLevelPreempt = true;
    featureTable->flags.ftr3dMidBatchPreempt = true;
    featureTable->flags.ftr3dObjectLevelPreempt = true;
    featureTable->flags.ftrPerCtxtPreemptionGranularityControl = true;
    featureTable->flags.ftrLCIA = true;
    featureTable->flags.ftrPPGTT = true;
    featureTable->flags.ftrIA32eGfxPTEs = true;
    featureTable->flags.ftrTranslationTable = true;
    featureTable->flags.ftrUserModeTranslationTable = true;
    featureTable->flags.ftrEnableGuC = true;
    featureTable->flags.ftrTileMappedResource = true;
    featureTable->flags.ftrULT = true;
    featureTable->flags.ftrAstcHdr2D = true;
    featureTable->flags.ftrAstcLdr2D = true;
    featureTable->flags.ftrTileY = true;
}




// Broxton (GEN9)
Platform BXT::platform = {
    PRODUCT_FAMILY::IGFX_BROXTON,
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
FeatureTable BXT::featureTable = {0};
void BXT::setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo) {
    FeatureTable* featureTable = hwInfo->featureTable;
    featureTable->flags.ftrGpGpuMidBatchPreempt = true;
    featureTable->flags.ftrGpGpuThreadGroupLevelPreempt = true;
    featureTable->flags.ftrL3IACoherency = true;
    featureTable->flags.ftrVEBOX = true;
    featureTable->flags.ftrULT = true;
    featureTable->flags.ftrGpGpuMidThreadLevelPreempt = true;
    featureTable->flags.ftr3dMidBatchPreempt = true;
    featureTable->flags.ftr3dObjectLevelPreempt = true;
    featureTable->flags.ftrPerCtxtPreemptionGranularityControl = true;
    featureTable->flags.ftrLCIA = true;
    featureTable->flags.ftrPPGTT = true;
    featureTable->flags.ftrIA32eGfxPTEs = true;
    featureTable->flags.ftrDisplayYTiling = true;
    featureTable->flags.ftrTranslationTable = true;
    featureTable->flags.ftrUserModeTranslationTable = true;
    featureTable->flags.ftrEnableGuC = true;
    featureTable->flags.ftrFbc = true;
    featureTable->flags.ftrFbc2AddressTranslation = true;
    featureTable->flags.ftrFbcBlitterTracking = true;
    featureTable->flags.ftrFbcCpuTracking = true;
    featureTable->flags.ftrTileY = true;
}




// Broadwell (GEN8)
Platform BDW::platform = {
    PRODUCT_FAMILY::IGFX_BROADWELL,
    PCH_PRODUCT_FAMILY::PCH_UNKNOWN,
    GFX_CORE_FAMILY::IGFX_GEN8_CORE,
    GFX_CORE_FAMILY::IGFX_GEN8_CORE,
    PLATFORM_TYPE::PLATFORM_NONE,
    0,
    0,
    0,
    0,
    GTTYPE::GTTYPE_UNDEFINED
};
FeatureTable BDW::featureTable = {0};
void BDW::setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo) {
    FeatureTable* featureTable = hwInfo->featureTable;
    featureTable->flags.ftrL3IACoherency = true;
    featureTable->flags.ftrPPGTT = true;
    featureTable->flags.ftrSVM = true;
    featureTable->flags.ftrIA32eGfxPTEs = true;
    featureTable->flags.ftrFbc = true;
    featureTable->flags.ftrFbc2AddressTranslation = true;
    featureTable->flags.ftrFbcBlitterTracking = true;
    featureTable->flags.ftrFbcCpuTracking = true;
    featureTable->flags.ftrTileY = true;
}












