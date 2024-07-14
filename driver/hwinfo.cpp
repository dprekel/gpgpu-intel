#include "hwinfo.h"


// Lunarlake
// Battlemage


// ------------------------------------------------------------------------------------------
// Ponte Vecchio
// ------------------------------------------------------------------------------------------
Platform PVC::platform = {
    PRODUCT_FAMILY::IGFX_PVC,
    PCH_PRODUCT_FAMILY::PCH_UNKNOWN,
    GFX_CORE_FAMILY::IGFX_XE_HPC_CORE,
    GFX_CORE_FAMILY::IGFX_XE_HPC_CORE,
    PLATFORM_TYPE::PLATFORM_NONE,
    0,
    0,
    0,
    0,
    GTTYPE::GTTYPE_UNDEFINED
};
FeatureTable PVC::featureTable = {0};
void PVC::setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo) {
    FeatureTable* featureTable = hwInfo->featureTable;
    featureTable->flags.ftrL3IACoherency = true;
    featureTable->flags.ftrLocalMemory = true;
    featureTable->flags.ftrLinearCCS = true;
    featureTable->flags.ftrFlatPhysCCS = true;
    featureTable->flags.ftrE2ECompression = false;
    featureTable->flags.ftrCCSNode = true;
    featureTable->flags.ftrCCSRing = true;
    featureTable->flags.ftrPPGTT = true;
    featureTable->flags.ftrSVM = true;
    featureTable->flags.ftrL3IACoherency = true;
    featureTable->flags.ftrIA32eGfxPTEs = true;
    featureTable->flags.ftrStandardMipTailFormat = true;
    featureTable->flags.ftrTranslationTable = true;
    featureTable->flags.ftrUserModeTranslationTable = true;
    featureTable->flags.ftrAstcHdr2D = true;
    featureTable->flags.ftrAstcLdr2D = true;
    featureTable->flags.ftrGpGpuMidBatchPreempt = true;
    featureTable->flags.ftrGpGpuThreadGroupLevelPreempt = true;
    featureTable->flags.ftrTileY = true;
    //featureTable->flags.ftrBcsInfo = maxNBitValue(9);
    featureTable->flags.ftrSupportsOcl21 = true;
    featureTable->flags.ftrSupportsOcl30 = true;
    featureTable->flags.ftrSupportsIndependentForwardProgress = true;
    featureTable->flags.ftrSupportsInteger64BitAtomics = true;
}
SystemInfo PvcHwConfig::gtSystemInfo = {0};
void PvcHwConfig::setupHardwareInfo(const HardwareInfo* hwInfo) {
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->ThreadCount = gtSysInfo->EUCount * PVC::threadsPerEu;
    gtSysInfo->SliceCount = 1;
    gtSysInfo->DualSubSliceCount = 6;
    gtSysInfo->L3CacheSizeInKb = 3840;
    gtSysInfo->L3BankCount = 8;
    gtSysInfo->MaxFillRate = 128;
    gtSysInfo->TotalVsThreads = 336;
    gtSysInfo->TotalHsThreads = 336;
    gtSysInfo->TotalDsThreads = 336;
    gtSysInfo->TotalGsThreads = 336;
    gtSysInfo->TotalPsThreadsWindowerRange = 64;
    gtSysInfo->CsrSizeInMb = 8;
    gtSysInfo->MaxEuPerSubSlice = PVC::maxEuPerSubSlice;
    gtSysInfo->MaxSlicesSupported = PVC::maxSlicesSupported;
    gtSysInfo->MaxSubSlicesSupported = PVC::maxSubSlicesSupported;
    gtSysInfo->MaxDualSubSlicesSupported = PVC::maxDualSubSlicesSupported;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
    gtSysInfo->NumThreadsPerEu = PVC::threadsPerEu;
    /*
    gtSysInfo->CCSInfo.IsValid = true;
    gtSysInfo->CCSInfo.NumberOfCCSEnabled = 1;
    gtSysInfo->CCSInfo.Instances.CCSEnableMask = 0b1;
    */
    setupFeatureAndWorkaroundTable(hwInfo);
}
const HardwareInfo PvcHwConfig::hwInfo = {
    &PVC::platform,
    &PVC::featureTable,
    &PvcHwConfig::gtSystemInfo
};




// ------------------------------------------------------------------------------------------
// Alchemist
// ------------------------------------------------------------------------------------------
Platform DG2::platform = {
    PRODUCT_FAMILY::IGFX_DG2,
    PCH_PRODUCT_FAMILY::PCH_UNKNOWN,
    GFX_CORE_FAMILY::IGFX_XE_HPG_CORE,
    GFX_CORE_FAMILY::IGFX_XE_HPG_CORE,
    PLATFORM_TYPE::PLATFORM_NONE,
    0,
    0,
    0,
    0,
    GTTYPE::GTTYPE_UNDEFINED
};
FeatureTable DG2::featureTable = {0};
void DG2::setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo) {
    FeatureTable* featureTable = hwInfo->featureTable;
    featureTable->flags.ftrL3IACoherency = true;
    featureTable->flags.ftrFlatPhysCCS = true;
    featureTable->flags.ftrPPGTT = true;
    featureTable->flags.ftrSVM = true;
    featureTable->flags.ftrIA32eGfxPTEs = true;
    featureTable->flags.ftrStandardMipTailFormat = true;
    featureTable->flags.ftrTranslationTable = true;
    featureTable->flags.ftrUserModeTranslationTable = true;
    featureTable->flags.ftrTileMappedResource = true;
    featureTable->flags.ftrFbc = true;
    featureTable->flags.ftrAstcHdr2D = true;
    featureTable->flags.ftrAstcLdr2D = true;
    featureTable->flags.ftrGpGpuMidBatchPreempt = true;
    featureTable->flags.ftrGpGpuThreadGroupLevelPreempt = true;
    featureTable->flags.ftrTileY = false;
    featureTable->flags.ftrLocalMemory = true;
    featureTable->flags.ftrLinearCCS = true;
    featureTable->flags.ftrE2ECompression = true;
    featureTable->flags.ftrCCSNode = true;
    featureTable->flags.ftrCCSRing = true;
    featureTable->flags.ftrUnified3DMediaCompressionFormats = true;
    //featureTable->flags.ftrTile64Optimization = true;
    featureTable->flags.ftrSupportsOcl21 = true;
    featureTable->flags.ftrSupportsOcl30 = true;
    featureTable->flags.ftrSupportsInteger64BitAtomics = true;
}
SystemInfo Dg2HwConfig::gtSystemInfo = {0};
void Dg2HwConfig::setupHardwareInfo(const HardwareInfo* hwInfo) {
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->ThreadCount = gtSysInfo->EUCount * DG2::threadsPerEu;
    gtSysInfo->SliceCount = 2;
    gtSysInfo->SubSliceCount = 8;
    gtSysInfo->DualSubSliceCount = gtSysInfo->SubSliceCount;
    gtSysInfo->L3CacheSizeInKb = 1;
    gtSysInfo->L3BankCount = 1;
    gtSysInfo->TotalVsThreads = 336;
    gtSysInfo->TotalHsThreads = 336;
    gtSysInfo->TotalDsThreads = 336;
    gtSysInfo->TotalGsThreads = 336;
    gtSysInfo->TotalPsThreadsWindowerRange = 64;
    gtSysInfo->CsrSizeInMb = 8;
    gtSysInfo->MaxEuPerSubSlice = DG2::maxEuPerSubSlice;
    gtSysInfo->MaxSlicesSupported = DG2::maxSlicesSupported;
    gtSysInfo->MaxSubSlicesSupported = DG2::maxSubSlicesSupported;
    gtSysInfo->MaxDualSubSlicesSupported = DG2::maxDualSubSlicesSupported;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
    gtSysInfo->NumThreadsPerEu = DG2::threadsPerEu;
    /*
    gtSysInfo->CCSInfo.IsValid = true;
    gtSysInfo->CCSInfo.NumberOfCCSEnabled = 1;
    gtSysInfo->CCSInfo.Instances.CCSEnableMask = 0b1;
    for (uint32_t slice = 0; slice < gtSysInfo->SliceCount; slice++) {
        gtSysInfo->SliceInfo[slice].Enabled = true;
    }
    */
    setupFeatureAndWorkaroundTable(hwInfo);
}
const HardwareInfo Dg2HwConfig::hwInfo = {
    &DG2::platform,
    &DG2::featureTable,
    &Dg2HwConfig::gtSystemInfo
};




// ------------------------------------------------------------------------------------------
// Meteorlake
// ------------------------------------------------------------------------------------------
Platform MTL::platform = {
    PRODUCT_FAMILY::IGFX_METEORLAKE,
    PCH_PRODUCT_FAMILY::PCH_UNKNOWN,
    GFX_CORE_FAMILY::IGFX_XE_HPG_CORE,
    GFX_CORE_FAMILY::IGFX_XE_HPG_CORE,
    PLATFORM_TYPE::PLATFORM_NONE,
    0,
    0,
    0,
    0,
    GTTYPE::GTTYPE_UNDEFINED
};
FeatureTable MTL::featureTable = {0};
void MTL::setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo) {
    FeatureTable* featureTable = hwInfo->featureTable;
    featureTable->flags.ftrL3IACoherency = true;
    featureTable->flags.ftrPPGTT = true;
    featureTable->flags.ftrSVM = true;
    featureTable->flags.ftrIA32eGfxPTEs = true;
    featureTable->flags.ftrStandardMipTailFormat = true;
    featureTable->flags.ftrTranslationTable = true;
    featureTable->flags.ftrUserModeTranslationTable = true;
    featureTable->flags.ftrTileMappedResource = true;
    featureTable->flags.ftrFbc = true;
    featureTable->flags.ftrAstcHdr2D = true;
    featureTable->flags.ftrAstcLdr2D = true;
    featureTable->flags.ftrGpGpuMidBatchPreempt = true;
    featureTable->flags.ftrGpGpuThreadGroupLevelPreempt = true;
    featureTable->flags.ftrTileY = false;
    featureTable->flags.ftrLinearCCS = true;
    featureTable->flags.ftrE2ECompression = true;
    featureTable->flags.ftrCCSNode = true;
    featureTable->flags.ftrCCSRing = true;
    //featureTable->flags.ftrTile64Optimization = true;
    featureTable->flags.ftrSupportsOcl21 = true;
    featureTable->flags.ftrSupportsOcl30 = true;
    featureTable->flags.ftrSupportsInteger64BitAtomics = true;
}
SystemInfo MtlHwConfig::gtSystemInfo = {0};
void MtlHwConfig::setupHardwareInfo(const HardwareInfo* hwInfo) {
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->ThreadCount = gtSysInfo->EUCount * MTL::threadsPerEu;
    gtSysInfo->SliceCount = 1;
    gtSysInfo->SubSliceCount = 8;
    gtSysInfo->DualSubSliceCount = gtSysInfo->SubSliceCount;
    gtSysInfo->L3BankCount = 1;
    gtSysInfo->TotalVsThreads = 336;
    gtSysInfo->TotalHsThreads = 336;
    gtSysInfo->TotalDsThreads = 336;
    gtSysInfo->TotalGsThreads = 336;
    gtSysInfo->TotalPsThreadsWindowerRange = 64;
    gtSysInfo->CsrSizeInMb = 8;
    gtSysInfo->MaxEuPerSubSlice = MTL::maxEuPerSubSlice;
    gtSysInfo->MaxSlicesSupported = MTL::maxSlicesSupported;
    gtSysInfo->MaxSubSlicesSupported = MTL::maxSubSlicesSupported;
    gtSysInfo->MaxDualSubSlicesSupported = MTL::maxDualSubSlicesSupported;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
    gtSysInfo->NumThreadsPerEu = MTL::threadsPerEu;
    /*
    gtSysInfo->CCSInfo.IsValid = true;
    gtSysInfo->CCSInfo.NumberOfCCSEnabled = 1;
    gtSysInfo->CCSInfo.Instances.CCSEnableMask = 0b1;
    */
    setupFeatureAndWorkaroundTable(hwInfo);
}
const HardwareInfo MtlHwConfig::hwInfo = {
    &MTL::platform,
    &MTL::featureTable,
    &MtlHwConfig::gtSystemInfo
};




// ------------------------------------------------------------------------------------------
// ARROWLAKE
// ------------------------------------------------------------------------------------------
Platform ARL::platform = {
    PRODUCT_FAMILY::IGFX_ARROWLAKE,
    PCH_PRODUCT_FAMILY::PCH_UNKNOWN,
    GFX_CORE_FAMILY::IGFX_XE_HPG_CORE,
    GFX_CORE_FAMILY::IGFX_XE_HPG_CORE,
    PLATFORM_TYPE::PLATFORM_NONE,
    0,
    0,
    0,
    0,
    GTTYPE::GTTYPE_UNDEFINED
};
FeatureTable ARL::featureTable = {0};
void ARL::setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo) {
    FeatureTable* featureTable = hwInfo->featureTable;
    featureTable->flags.ftrL3IACoherency = true;
    featureTable->flags.ftrPPGTT = true;
    featureTable->flags.ftrSVM = true;
    featureTable->flags.ftrIA32eGfxPTEs = true;
    featureTable->flags.ftrStandardMipTailFormat = true;
    featureTable->flags.ftrTranslationTable = true;
    featureTable->flags.ftrUserModeTranslationTable = true;
    featureTable->flags.ftrTileMappedResource = true;
    featureTable->flags.ftrFbc = true;
    featureTable->flags.ftrTileY = true;
    featureTable->flags.ftrAstcHdr2D = true;
    featureTable->flags.ftrAstcLdr2D = true;
    featureTable->flags.ftrGpGpuMidBatchPreempt = true;
    featureTable->flags.ftrGpGpuThreadGroupLevelPreempt = true;
    featureTable->flags.ftrSupportsOcl21 = true;
    featureTable->flags.ftrSupportsOcl30 = true;
    featureTable->flags.ftrSupportsInteger64BitAtomics = true;
}
SystemInfo ArlHwConfig::gtSystemInfo = {0};
void ArlHwConfig::setupHardwareInfo(const HardwareInfo* hwInfo) {
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->ThreadCount = gtSysInfo->EUCount * ARL::threadsPerEu;
    gtSysInfo->L3CacheSizeInKb = 1;
    gtSysInfo->L3BankCount = 1;
    gtSysInfo->TotalPsThreadsWindowerRange = 64;
    gtSysInfo->CsrSizeInMb = 8;
    gtSysInfo->TotalVsThreads = 336;
    gtSysInfo->TotalHsThreads = 336;
    gtSysInfo->TotalDsThreads = 336;
    gtSysInfo->TotalGsThreads = 336;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
    gtSysInfo->NumThreadsPerEu = ARL::threadsPerEu;
    /*
    gtSysInfo->CCSInfo.IsValid = true;
    gtSysInfo->CCSInfo.NumberOfCCSEnabled = 1;
    gtSysInfo->CCSInfo.Instances.CCSEnableMask = 0b1;
    */
    setupFeatureAndWorkaroundTable(hwInfo);
}
const HardwareInfo ArlHwConfig::hwInfo = {
    &ARL::platform,
    &ARL::featureTable,
    &ArlHwConfig::gtSystemInfo
};




// ------------------------------------------------------------------------------------------
// Tigerlake (GEN12LP)
// ------------------------------------------------------------------------------------------
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
    featureTable->flags.ftrSupportsOcl21 = true;
    featureTable->flags.ftrSupportsOcl30 = true;
    featureTable->flags.ftrSupportsInteger64BitAtomics = true;
}
SystemInfo TgllpHw1x6x16::gtSystemInfo = {0};
void TgllpHw1x6x16::setupHardwareInfo(const HardwareInfo* hwInfo) {
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
    gtSysInfo->MaxSubSlicesSupported = TGLLP::maxSubSlicesSupported;
    gtSysInfo->MaxDualSubSlicesSupported = TGLLP::maxDualSubSlicesSupported;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
    gtSysInfo->NumThreadsPerEu = TGLLP::threadsPerEu;
    /*
    gtSysInfo->CCSInfo.IsValid = true;
    gtSysInfo->CCSInfo.NumberOfCCSEnabled = 1;
    gtSysInfo->CCSInfo.Instances.CCSEnableMask = 0b1;
    */
    setupFeatureAndWorkaroundTable(hwInfo);
}
const HardwareInfo TgllpHw1x6x16::hwInfo = {
    &TGLLP::platform,
    &TGLLP::featureTable,
    &TgllpHw1x6x16::gtSystemInfo
};
SystemInfo TgllpHw1x2x16::gtSystemInfo = {0};
void TgllpHw1x2x16::setupHardwareInfo(const HardwareInfo* hwInfo) {
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
    gtSysInfo->MaxSubSlicesSupported = TGLLP::maxSubSlicesSupported;
    gtSysInfo->MaxDualSubSlicesSupported = TGLLP::maxDualSubSlicesSupported;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
    /*
    gtSysInfo->CCSInfo.IsValid = true;
    gtSysInfo->CCSInfo.NumberOfCCSEnabled = 1;
    gtSysInfo->CCSInfo.Instances.CCSEnableMask = 0b1;
    */
    setupFeatureAndWorkaroundTable(hwInfo);
}
const HardwareInfo TgllpHw1x2x16::hwInfo = {
    &TGLLP::platform,
    &TGLLP::featureTable,
    &TgllpHw1x2x16::gtSystemInfo
};




// ------------------------------------------------------------------------------------------
// DG1 (GEN12LP)
// ------------------------------------------------------------------------------------------
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
    //featureTable->flags.ftrBcsInfo = maxNBitValue(1);
    featureTable->flags.ftrSupportsOcl21 = true;
    featureTable->flags.ftrSupportsOcl30 = true;
    featureTable->flags.ftrSupportsInteger64BitAtomics = true;
}
SystemInfo Dg1HwConfig::gtSystemInfo = {0};
void Dg1HwConfig::setupHardwareInfo(const HardwareInfo* hwInfo) {
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->ThreadCount = gtSysInfo->EUCount * DG1::threadsPerEu;
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
    gtSysInfo->NumThreadsPerEu = DG1::threadsPerEu;
    /*
    gtSysInfo->CCSInfo.IsValid = true;
    gtSysInfo->CCSInfo.NumberOfCCSEnabled = 1;
    gtSysInfo->CCSInfo.Instances.CCSEnableMask = 0b1;
    */
    setupFeatureAndWorkaroundTable(hwInfo);
}
const HardwareInfo Dg1HwConfig::hwInfo = {
    &DG1::platform,
    &DG1::featureTable,
    &Dg1HwConfig::gtSystemInfo
};




// ------------------------------------------------------------------------------------------
// Rocketlake (GEN12)
// ------------------------------------------------------------------------------------------
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
    featureTable->flags.ftrSupportsOcl21 = true;
    featureTable->flags.ftrSupportsOcl30 = true;
    featureTable->flags.ftrSupportsInteger64BitAtomics = true;
}
SystemInfo RklHwConfig::gtSystemInfo = {0};
void RklHwConfig::setupHardwareInfo(const HardwareInfo* hwInfo) {
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
    gtSysInfo->NumThreadsPerEu = RKL::threadsPerEu;
    /*
    gtSysInfo->CCSInfo.IsValid = true;
    gtSysInfo->CCSInfo.NumberOfCCSEnabled = 1;
    gtSysInfo->CCSInfo.Instances.CCSEnableMask = 0b1;
    */
    setupFeatureAndWorkaroundTable(hwInfo);
}
const HardwareInfo RklHwConfig::hwInfo = {
    &RKL::platform,
    &RKL::featureTable,
    &RklHwConfig::gtSystemInfo
};




// ------------------------------------------------------------------------------------------
// Alderlake_S (GEN12LP)
// ------------------------------------------------------------------------------------------
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
    featureTable->flags.ftrFbc = true;
    featureTable->flags.ftrTileY = true;
    featureTable->flags.ftrAstcHdr2D = true;
    featureTable->flags.ftrAstcLdr2D = true;
    featureTable->flags.ftrGpGpuMidBatchPreempt = true;
    featureTable->flags.ftrGpGpuThreadGroupLevelPreempt = true;
    featureTable->flags.ftrSupportsOcl21 = true;
    featureTable->flags.ftrSupportsOcl30 = true;
    featureTable->flags.ftrSupportsInteger64BitAtomics = true;
}
SystemInfo AdlsHwConfig::gtSystemInfo = {0};
void AdlsHwConfig::setupHardwareInfo(const HardwareInfo* hwInfo) {
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
    gtSysInfo->NumThreadsPerEu = ADLS::threadsPerEu;
    /*
    gtSysInfo->CCSInfo.IsValid = true;
    gtSysInfo->CCSInfo.NumberOfCCSEnabled = 1;
    gtSysInfo->CCSInfo.Instances.CCSEnableMask = 0b1;
    */
    setupFeatureAndWorkaroundTable(hwInfo);
}
const HardwareInfo AdlsHwConfig::hwInfo = {
    &ADLS::platform,
    &ADLS::featureTable,
    &AdlsHwConfig::gtSystemInfo
};




// ------------------------------------------------------------------------------------------
// Alderlake_N (GEN12LP)
// ------------------------------------------------------------------------------------------
Platform ADLN::platform = {
    PRODUCT_FAMILY::IGFX_ALDERLAKE_N,
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
FeatureTable ADLN::featureTable = {0};
void ADLN::setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo) {
    FeatureTable* featureTable = hwInfo->featureTable;
    featureTable->flags.ftrL3IACoherency = true;
    featureTable->flags.ftrPPGTT = true;
    featureTable->flags.ftrSVM = true;
    featureTable->flags.ftrIA32eGfxPTEs = true;
    featureTable->flags.ftrStandardMipTailFormat = true;
    featureTable->flags.ftrTranslationTable = true;
    featureTable->flags.ftrUserModeTranslationTable = true;
    featureTable->flags.ftrTileMappedResource = true;
    featureTable->flags.ftrFbc = true;
    featureTable->flags.ftrTileY = true;
    featureTable->flags.ftrAstcHdr2D = true;
    featureTable->flags.ftrAstcLdr2D = true;
    featureTable->flags.ftrGpGpuMidBatchPreempt = true;
    featureTable->flags.ftrGpGpuThreadGroupLevelPreempt = true;
    featureTable->flags.ftrSupportsOcl21 = true;
    featureTable->flags.ftrSupportsOcl30 = true;
    featureTable->flags.ftrSupportsInteger64BitAtomics = true;
}
SystemInfo AdlnHwConfig::gtSystemInfo = {0};
void AdlnHwConfig::setupHardwareInfo(const HardwareInfo* hwInfo) {
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->ThreadCount = gtSysInfo->EUCount * ADLN::threadsPerEu;
    gtSysInfo->L3CacheSizeInKb = 1920;
    gtSysInfo->L3BankCount = 4;
    gtSysInfo->MaxFillRate = 8;
    gtSysInfo->TotalPsThreadsWindowerRange = 64;
    gtSysInfo->CsrSizeInMb = 8;
    gtSysInfo->MaxEuPerSubSlice = ADLN::maxEuPerSubSlice;
    gtSysInfo->MaxSlicesSupported = ADLN::maxSlicesSupported;
    gtSysInfo->MaxSubSlicesSupported = ADLN::maxSubSlicesSupported;
    gtSysInfo->MaxDualSubSlicesSupported = ADLN::maxDualSubSlicesSupported;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
    gtSysInfo->NumThreadsPerEu = ADLN::threadsPerEu;
    /*
    gtSysInfo->CCSInfo.IsValid = true;
    gtSysInfo->CCSInfo.NumberOfCCSEnabled = 1;
    gtSysInfo->CCSInfo.Instances.CCSEnableMask = 0b1;
    */
    setupFeatureAndWorkaroundTable(hwInfo);
}
const HardwareInfo AdlnHwConfig::hwInfo = {
    &ADLN::platform,
    &ADLN::featureTable,
    &AdlnHwConfig::gtSystemInfo
};




// ------------------------------------------------------------------------------------------
// Alderlake_P (GEN12LP)
// ------------------------------------------------------------------------------------------
Platform ADLP::platform = {
    PRODUCT_FAMILY::IGFX_ALDERLAKE_P,
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
FeatureTable ADLP::featureTable = {0};
void ADLP::setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo) {
    FeatureTable* featureTable = hwInfo->featureTable;
    featureTable->flags.ftrL3IACoherency = true;
    featureTable->flags.ftrPPGTT = true;
    featureTable->flags.ftrSVM = true;
    featureTable->flags.ftrIA32eGfxPTEs = true;
    featureTable->flags.ftrStandardMipTailFormat = true;
    featureTable->flags.ftrTranslationTable = true;
    featureTable->flags.ftrUserModeTranslationTable = true;
    featureTable->flags.ftrTileMappedResource = true;
    featureTable->flags.ftrFbc = true;
    featureTable->flags.ftrTileY = true;
    featureTable->flags.ftrAstcHdr2D = true;
    featureTable->flags.ftrAstcLdr2D = true;
    featureTable->flags.ftrGpGpuMidBatchPreempt = true;
    featureTable->flags.ftrGpGpuThreadGroupLevelPreempt = true;
    featureTable->flags.ftrSupportsOcl21 = true;
    featureTable->flags.ftrSupportsOcl30 = true;
    featureTable->flags.ftrSupportsInteger64BitAtomics = true;
}
SystemInfo AdlpHwConfig::gtSystemInfo = {0};
void AdlpHwConfig::setupHardwareInfo(const HardwareInfo* hwInfo) {
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->ThreadCount = gtSysInfo->EUCount * ADLP::threadsPerEu;
    gtSysInfo->L3CacheSizeInKb = 1;
    gtSysInfo->L3BankCount = 1;
    gtSysInfo->TotalPsThreadsWindowerRange = 64;
    gtSysInfo->CsrSizeInMb = 8;
    gtSysInfo->MaxEuPerSubSlice = ADLP::maxEuPerSubSlice;
    gtSysInfo->MaxSlicesSupported = ADLP::maxSlicesSupported;
    gtSysInfo->MaxSubSlicesSupported = ADLP::maxSubSlicesSupported;
    gtSysInfo->MaxDualSubSlicesSupported = ADLP::maxDualSubSlicesSupported;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
    gtSysInfo->NumThreadsPerEu = ADLP::threadsPerEu;
    /*
    gtSysInfo->CCSInfo.IsValid = true;
    gtSysInfo->CCSInfo.NumberOfCCSEnabled = 1;
    gtSysInfo->CCSInfo.Instances.CCSEnableMask = 0b1;
    */
    setupFeatureAndWorkaroundTable(hwInfo);
}
const HardwareInfo AdlpHwConfig::hwInfo = {
    &ADLP::platform,
    &ADLP::featureTable,
    &AdlpHwConfig::gtSystemInfo
};




// ------------------------------------------------------------------------------------------
// Icelake_LP (GEN11)
// ------------------------------------------------------------------------------------------
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
    featureTable->flags.ftrSupportsOcl21 = true;
    featureTable->flags.ftrSupportsOcl30 = true;
    featureTable->flags.ftrSupportsIndependentForwardProgress = true;
    featureTable->flags.ftrSupportsInteger64BitAtomics = true;
}
void ICLLP::setupHardwareInfoBase(const HardwareInfo* hwInfo) {
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->ThreadCount = gtSysInfo->EUCount * ICLLP::threadsPerEu;
    gtSysInfo->TotalVsThreads = 364;
    gtSysInfo->TotalHsThreads = 224;
    gtSysInfo->TotalDsThreads = 364;
    gtSysInfo->TotalGsThreads = 224;
    gtSysInfo->TotalPsThreadsWindowerRange = 128;
    gtSysInfo->CsrSizeInMb = 5;
    gtSysInfo->MaxEuPerSubSlice = ICLLP::maxEuPerSubSlice;
    gtSysInfo->MaxSlicesSupported = ICLLP::maxSlicesSupported;
    gtSysInfo->MaxSubSlicesSupported = ICLLP::maxSubSlicesSupported;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
    gtSysInfo->NumThreadsPerEu = ICLLP::threadsPerEu;
    setupFeatureAndWorkaroundTable(hwInfo);
}
SystemInfo IcllpHw1x4x8::gtSystemInfo = {0};
void IcllpHw1x4x8::setupHardwareInfo(const HardwareInfo* hwInfo) {
    setupHardwareInfoBase(hwInfo);
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->SliceCount = 1;
    gtSysInfo->L3CacheSizeInKb = 2304;
    gtSysInfo->L3BankCount = 6;
    gtSysInfo->MaxFillRate = 8;
}
const HardwareInfo IcllpHw1x4x8::hwInfo = {
    &ICLLP::platform,
    &ICLLP::featureTable,
    &IcllpHw1x4x8::gtSystemInfo
};
SystemInfo IcllpHw1x6x8::gtSystemInfo = {0};
void IcllpHw1x6x8::setupHardwareInfo(const HardwareInfo* hwInfo) {
    setupHardwareInfoBase(hwInfo);
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->ThreadCount = gtSysInfo->EUCount * ICLLP::threadsPerEu;
    gtSysInfo->SliceCount = 1;
    gtSysInfo->L3CacheSizeInKb = 2304;
    gtSysInfo->L3BankCount = 6;
    gtSysInfo->MaxFillRate = 8;
}
const HardwareInfo IcllpHw1x6x8::hwInfo = {
    &ICLLP::platform,
    &ICLLP::featureTable,
    &IcllpHw1x6x8::gtSystemInfo
};
SystemInfo IcllpHw1x8x8::gtSystemInfo = {0};
void IcllpHw1x8x8::setupHardwareInfo(const HardwareInfo* hwInfo) {
    setupHardwareInfoBase(hwInfo);
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->ThreadCount = gtSysInfo->EUCount * ICLLP::threadsPerEu;
    gtSysInfo->SliceCount = 1;
    gtSysInfo->L3CacheSizeInKb = 3072;
    gtSysInfo->L3BankCount = 8;
    gtSysInfo->MaxFillRate = 16;
}
const HardwareInfo IcllpHw1x8x8::hwInfo = {
    &ICLLP::platform,
    &ICLLP::featureTable,
    &IcllpHw1x8x8::gtSystemInfo
};




// ------------------------------------------------------------------------------------------
// Lakefield (GEN11)
// ------------------------------------------------------------------------------------------
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
    featureTable->flags.ftrSupportsOcl30 = true;
}
SystemInfo LkfHw1x8x8::gtSystemInfo = {0};
void LkfHw1x8x8::setupHardwareInfo(const HardwareInfo* hwInfo) {
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->ThreadCount = gtSysInfo->EUCount * LKF::threadsPerEu;
    gtSysInfo->DualSubSliceCount = gtSysInfo->SubSliceCount;
    gtSysInfo->L3CacheSizeInKb = 2560;
    gtSysInfo->L3BankCount = 8;
    gtSysInfo->MaxFillRate = 16;
    gtSysInfo->TotalVsThreads = 448;
    gtSysInfo->TotalHsThreads = 448;
    gtSysInfo->TotalDsThreads = 448;
    gtSysInfo->TotalGsThreads = 448;
    gtSysInfo->TotalPsThreadsWindowerRange = 64;
    gtSysInfo->CsrSizeInMb = 8;
    gtSysInfo->MaxEuPerSubSlice = LKF::maxEuPerSubSlice;
    gtSysInfo->MaxSlicesSupported = LKF::maxSlicesSupported;
    gtSysInfo->MaxSubSlicesSupported = 2;
    gtSysInfo->MaxDualSubSlicesSupported = 2;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
    gtSysInfo->NumThreadsPerEu = LKF::threadsPerEu;
    /*
    gtSysInfo->CCSInfo.IsValid = true;
    gtSysInfo->CCSInfo.NumberOfCCSEnabled = 1;
    gtSysInfo->CCSInfo.Instances.CCSEnableMask = 0b1;
    */
    setupFeatureAndWorkaroundTable(hwInfo);
}
const HardwareInfo LkfHw1x8x8::hwInfo = {
    &LKF::platform,
    &LKF::featureTable,
    &LkfHw1x8x8::gtSystemInfo
};




// ------------------------------------------------------------------------------------------
// Elkhart Lake (GEN11)
// ------------------------------------------------------------------------------------------
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
    featureTable->flags.ftrSupportsOcl30 = true;
}
SystemInfo EhlHwConfig::gtSystemInfo = {0};
void EhlHwConfig::setupHardwareInfo(const HardwareInfo* hwInfo) {
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->ThreadCount = gtSysInfo->EUCount * EHL::threadsPerEu;
    gtSysInfo->DualSubSliceCount = gtSysInfo->SubSliceCount;
    gtSysInfo->L3CacheSizeInKb = 2560;
    gtSysInfo->L3BankCount = 8;
    gtSysInfo->MaxFillRate = 16;
    gtSysInfo->TotalVsThreads = 448;
    gtSysInfo->TotalHsThreads = 448;
    gtSysInfo->TotalDsThreads = 448;
    gtSysInfo->TotalGsThreads = 448;
    gtSysInfo->TotalPsThreadsWindowerRange = 64;
    gtSysInfo->CsrSizeInMb = 8;
    gtSysInfo->MaxEuPerSubSlice = EHL::maxEuPerSubSlice;
    gtSysInfo->MaxSlicesSupported = EHL::maxSlicesSupported;
    gtSysInfo->MaxSubSlicesSupported = 2;
    gtSysInfo->MaxDualSubSlicesSupported = 2;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
    gtSysInfo->NumThreadsPerEu = EHL::threadsPerEu;
    /*
    gtSysInfo->CCSInfo.IsValid = true;
    gtSysInfo->CCSInfo.NumberOfCCSEnabled = 1;
    gtSysInfo->CCSInfo.Instances.CCSEnableMask = 0b1;
    */
    setupFeatureAndWorkaroundTable(hwInfo);
}
const HardwareInfo EhlHwConfig::hwInfo = {
    &EHL::platform,
    &EHL::featureTable,
    &EhlHwConfig::gtSystemInfo
};




// ------------------------------------------------------------------------------------------
// Skylake (GEN9)
// ------------------------------------------------------------------------------------------
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
    featureTable->flags.ftrSupportsOcl21 = true;
    featureTable->flags.ftrSupportsOcl30 = true;
    featureTable->flags.ftrSupportsIndependentForwardProgress = true;
    featureTable->flags.ftrSupportsInteger64BitAtomics = true;
}
void SKL::setupHardwareInfoBase(const HardwareInfo* hwInfo) {
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->ThreadCount = gtSysInfo->EUCount * SKL::threadsPerEu;
    gtSysInfo->TotalVsThreads = 336;
    gtSysInfo->TotalHsThreads = 336;
    gtSysInfo->TotalDsThreads = 336;
    gtSysInfo->TotalGsThreads = 336;
    gtSysInfo->TotalPsThreadsWindowerRange = 64;
    gtSysInfo->CsrSizeInMb = 8;
    gtSysInfo->MaxEuPerSubSlice = SKL::maxEuPerSubSlice;
    gtSysInfo->MaxSlicesSupported = SKL::maxSlicesSupported;
    gtSysInfo->MaxSubSlicesSupported = SKL::maxSubSlicesSupported;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
    gtSysInfo->NumThreadsPerEu = SKL::threadsPerEu;
    setupFeatureAndWorkaroundTable(hwInfo);
}
SystemInfo SklHw1x2x6::gtSystemInfo = {0};
void SklHw1x2x6::setupHardwareInfo(const HardwareInfo* hwInfo) {
    setupHardwareInfoBase(hwInfo);
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->SliceCount = 1;
    gtSysInfo->L3CacheSizeInKb = 384;
    gtSysInfo->L3BankCount = 2;
    gtSysInfo->MaxFillRate = 8;
}
const HardwareInfo SklHw1x2x6::hwInfo = {
    &SKL::platform,
    &SKL::featureTable,
    &SklHw1x2x6::gtSystemInfo
};
SystemInfo SklHw1x3x6::gtSystemInfo = {0};
void SklHw1x3x6::setupHardwareInfo(const HardwareInfo* hwInfo) {
    setupHardwareInfoBase(hwInfo);
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->SliceCount = 1;
    gtSysInfo->L3CacheSizeInKb = 768;
    gtSysInfo->L3BankCount = 4;
    gtSysInfo->MaxFillRate = 8;
}
const HardwareInfo SklHw1x3x6::hwInfo = {
    &SKL::platform,
    &SKL::featureTable,
    &SklHw1x3x6::gtSystemInfo,
};
SystemInfo SklHw1x3x8::gtSystemInfo = {0};
void SklHw1x3x8::setupHardwareInfo(const HardwareInfo* hwInfo) {
    setupHardwareInfoBase(hwInfo);
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->SliceCount = 1;
    gtSysInfo->L3CacheSizeInKb = 768;
    gtSysInfo->L3BankCount = 4;
    gtSysInfo->MaxFillRate = 8;
}
const HardwareInfo SklHw1x3x8::hwInfo = {
    &SKL::platform,
    &SKL::featureTable,
    &SklHw1x3x8::gtSystemInfo,
};
SystemInfo SklHw2x3x8::gtSystemInfo = {0};
void SklHw2x3x8::setupHardwareInfo(const HardwareInfo* hwInfo) {
    setupHardwareInfoBase(hwInfo);
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->ThreadCount = gtSysInfo->EUCount * SKL::threadsPerEu;
    gtSysInfo->SliceCount = 2;
    gtSysInfo->L3CacheSizeInKb = 1536;
    gtSysInfo->L3BankCount = 8;
    gtSysInfo->MaxFillRate = 16;
}
const HardwareInfo SklHw2x3x8::hwInfo = {
    &SKL::platform,
    &SKL::featureTable,
    &SklHw2x3x8::gtSystemInfo,
};
SystemInfo SklHw3x3x8::gtSystemInfo = {0};
void SklHw3x3x8::setupHardwareInfo(const HardwareInfo* hwInfo) {
    setupHardwareInfoBase(hwInfo);
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->SliceCount = 3;
    gtSysInfo->L3CacheSizeInKb = 2304;
    gtSysInfo->L3BankCount = 12;
    gtSysInfo->MaxFillRate = 24;
}
const HardwareInfo SklHw3x3x8::hwInfo = {
    &SKL::platform,
    &SKL::featureTable,
    &SklHw3x3x8::gtSystemInfo,
};



// ------------------------------------------------------------------------------------------
// Kabylake (GEN9)
// ------------------------------------------------------------------------------------------
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
    featureTable->flags.ftrSupportsOcl21 = true;
    featureTable->flags.ftrSupportsOcl30 = true;
    featureTable->flags.ftrSupportsIndependentForwardProgress = true;
    featureTable->flags.ftrSupportsInteger64BitAtomics = true;
}
void KBL::setupHardwareInfoBase(const HardwareInfo* hwInfo) {
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->ThreadCount = gtSysInfo->EUCount * KBL::threadsPerEu;
    gtSysInfo->TotalVsThreads = 336;
    gtSysInfo->TotalHsThreads = 336;
    gtSysInfo->TotalDsThreads = 336;
    gtSysInfo->TotalGsThreads = 336;
    gtSysInfo->TotalPsThreadsWindowerRange = 64;
    gtSysInfo->CsrSizeInMb = 8;
    gtSysInfo->MaxEuPerSubSlice = KBL::maxEuPerSubSlice;
    gtSysInfo->MaxSlicesSupported = KBL::maxSlicesSupported;
    gtSysInfo->MaxSubSlicesSupported = KBL::maxSubSlicesSupported;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
    gtSysInfo->NumThreadsPerEu = KBL::threadsPerEu;
    setupFeatureAndWorkaroundTable(hwInfo);
}
SystemInfo KblHw1x2x6::gtSystemInfo = {0};
void KblHw1x2x6::setupHardwareInfo(const HardwareInfo* hwInfo) {
    setupHardwareInfoBase(hwInfo);
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->SliceCount = 1;
    gtSysInfo->L3CacheSizeInKb = 384;
    gtSysInfo->L3BankCount = 2;
    gtSysInfo->MaxFillRate = 8;
}
const HardwareInfo KblHw1x2x6::hwInfo = {
    &KBL::platform,
    &KBL::featureTable,
    &KblHw1x2x6::gtSystemInfo,
};
SystemInfo KblHw1x3x6::gtSystemInfo = {0};
void KblHw1x3x6::setupHardwareInfo(const HardwareInfo* hwInfo) {
    setupHardwareInfoBase(hwInfo);
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->SliceCount = 1;
    gtSysInfo->L3CacheSizeInKb = 768;
    gtSysInfo->L3BankCount = 4;
    gtSysInfo->MaxFillRate = 8;
}
const HardwareInfo KblHw1x3x6::hwInfo = {
    &KBL::platform,
    &KBL::featureTable,
    &KblHw1x3x6::gtSystemInfo,
};
SystemInfo KblHw1x3x8::gtSystemInfo = {0};
void KblHw1x3x8::setupHardwareInfo(const HardwareInfo* hwInfo) {
    setupHardwareInfoBase(hwInfo);
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->SliceCount = 1;
    gtSysInfo->L3CacheSizeInKb = 768;
    gtSysInfo->L3BankCount = 4;
    gtSysInfo->MaxFillRate = 8;
}
const HardwareInfo KblHw1x3x8::hwInfo = {
    &KBL::platform,
    &KBL::featureTable,
    &KblHw1x3x8::gtSystemInfo,
};
SystemInfo KblHw2x3x8::gtSystemInfo = {0};
void KblHw2x3x8::setupHardwareInfo(const HardwareInfo* hwInfo) {
    setupHardwareInfoBase(hwInfo);
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->SliceCount = 2;
    gtSysInfo->L3CacheSizeInKb = 1536;
    gtSysInfo->L3BankCount = 8;
    gtSysInfo->MaxFillRate = 16;
}
const HardwareInfo KblHw2x3x8::hwInfo = {
    &KBL::platform,
    &KBL::featureTable,
    &KblHw2x3x8::gtSystemInfo,
};
SystemInfo KblHw3x3x8::gtSystemInfo = {0};
void KblHw3x3x8::setupHardwareInfo(const HardwareInfo* hwInfo) {
    setupHardwareInfoBase(hwInfo);
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->SliceCount = 3;
    gtSysInfo->L3CacheSizeInKb = 2304;
    gtSysInfo->L3BankCount = 12;
    gtSysInfo->MaxFillRate = 23;
}
const HardwareInfo KblHw3x3x8::hwInfo = {
    &KBL::platform,
    &KBL::featureTable,
    &KblHw3x3x8::gtSystemInfo,
};




// ------------------------------------------------------------------------------------------
// Coffeelake (GEN9)
// ------------------------------------------------------------------------------------------
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
FeatureTable CFL::featureTable = {0};
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
    featureTable->flags.ftrSupportsOcl21 = true;
    featureTable->flags.ftrSupportsOcl30 = true;
    featureTable->flags.ftrSupportsIndependentForwardProgress = true;
    featureTable->flags.ftrSupportsInteger64BitAtomics = true;
}
void CFL::setupHardwareInfoBase(const HardwareInfo* hwInfo) {
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->ThreadCount = gtSysInfo->EUCount * CFL::threadsPerEu;
    gtSysInfo->TotalVsThreads = 336;
    gtSysInfo->TotalHsThreads = 336;
    gtSysInfo->TotalDsThreads = 336;
    gtSysInfo->TotalGsThreads = 336;
    gtSysInfo->TotalPsThreadsWindowerRange = 64;
    gtSysInfo->CsrSizeInMb = 8;
    gtSysInfo->MaxEuPerSubSlice = CFL::maxEuPerSubSlice;
    gtSysInfo->MaxSlicesSupported = CFL::maxSlicesSupported;
    gtSysInfo->MaxSubSlicesSupported = CFL::maxSubSlicesSupported;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
    gtSysInfo->NumThreadsPerEu = CFL::threadsPerEu;
    setupFeatureAndWorkaroundTable(hwInfo);
}
SystemInfo CflHw1x2x6::gtSystemInfo = {0};
void CflHw1x2x6::setupHardwareInfo(const HardwareInfo* hwInfo) {
    setupHardwareInfoBase(hwInfo);
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->SliceCount = 1;
    gtSysInfo->L3CacheSizeInKb = 384;
    gtSysInfo->L3BankCount = 2;
    gtSysInfo->MaxFillRate = 8;
}
const HardwareInfo CflHw1x2x6::hwInfo = {
    &CFL::platform,
    &CFL::featureTable,
    &CflHw1x2x6::gtSystemInfo,
};
SystemInfo CflHw1x3x6::gtSystemInfo = {0};
void CflHw1x3x6::setupHardwareInfo(const HardwareInfo* hwInfo) {
    setupHardwareInfoBase(hwInfo);
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->SliceCount = 1;
    gtSysInfo->L3CacheSizeInKb = 768;
    gtSysInfo->L3BankCount = 4;
    gtSysInfo->MaxFillRate = 8;
}
const HardwareInfo CflHw1x3x6::hwInfo = {
    &CFL::platform,
    &CFL::featureTable,
    &CflHw1x3x6::gtSystemInfo,
};
SystemInfo CflHw1x3x8::gtSystemInfo = {0};
void CflHw1x3x8::setupHardwareInfo(const HardwareInfo* hwInfo) {
    setupHardwareInfoBase(hwInfo);
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->ThreadCount = gtSysInfo->EUCount * CFL::threadsPerEu;
    gtSysInfo->SliceCount = 1;
    gtSysInfo->L3CacheSizeInKb = 768;
    gtSysInfo->L3BankCount = 4;
    gtSysInfo->MaxFillRate = 8;
}
const HardwareInfo CflHw1x3x8::hwInfo = {
    &CFL::platform,
    &CFL::featureTable,
    &CflHw1x3x8::gtSystemInfo,
};
SystemInfo CflHw2x3x8::gtSystemInfo = {0};
void CflHw2x3x8::setupHardwareInfo(const HardwareInfo* hwInfo) {
    setupHardwareInfoBase(hwInfo);
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->ThreadCount = gtSysInfo->EUCount * CFL::threadsPerEu;
    gtSysInfo->SliceCount = 1;
    gtSysInfo->L3CacheSizeInKb = 768;
    gtSysInfo->L3BankCount = 4;
    gtSysInfo->MaxFillRate = 8;
}
const HardwareInfo CflHw2x3x8::hwInfo = {
    &CFL::platform,
    &CFL::featureTable,
    &CflHw2x3x8::gtSystemInfo,
};




// ------------------------------------------------------------------------------------------
// Gemini Lake (GEN9)
// ------------------------------------------------------------------------------------------
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
    featureTable->flags.ftrSupportsOcl30 = true;
}
void GLK::setupHardwareInfoBase(const HardwareInfo* hwInfo) {
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->ThreadCount = gtSysInfo->EUCount * GLK::threadsPerEu;
    gtSysInfo->TotalVsThreads = 112;
    gtSysInfo->TotalHsThreads = 112;
    gtSysInfo->TotalDsThreads = 112;
    gtSysInfo->TotalGsThreads = 112;
    gtSysInfo->TotalPsThreadsWindowerRange = 64;
    gtSysInfo->CsrSizeInMb = 8;
    gtSysInfo->MaxEuPerSubSlice = GLK::maxEuPerSubSlice;
    gtSysInfo->MaxSlicesSupported = GLK::maxSlicesSupported;
    gtSysInfo->MaxSubSlicesSupported = GLK::maxSubSlicesSupported;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
    gtSysInfo->NumThreadsPerEu = GLK::threadsPerEu;
    setupFeatureAndWorkaroundTable(hwInfo);
}
SystemInfo GlkHw1x2x6::gtSystemInfo = {0};
void GlkHw1x2x6::setupHardwareInfo(const HardwareInfo* hwInfo) {
    setupHardwareInfoBase(hwInfo);
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->SliceCount = 1;
    gtSysInfo->L3CacheSizeInKb = 384;
    gtSysInfo->L3BankCount = 1;
    gtSysInfo->MaxFillRate = 8;
}
const HardwareInfo GlkHw1x2x6::hwInfo = {
    &GLK::platform,
    &GLK::featureTable,
    &GlkHw1x2x6::gtSystemInfo,
};
SystemInfo GlkHw1x3x6::gtSystemInfo = {0};
void GlkHw1x3x6::setupHardwareInfo(const HardwareInfo* hwInfo) {
    setupHardwareInfoBase(hwInfo);
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->SliceCount = 1;
    gtSysInfo->L3CacheSizeInKb = 384;
    gtSysInfo->L3BankCount = 1;
    gtSysInfo->MaxFillRate = 8;
}
const HardwareInfo GlkHw1x3x6::hwInfo = {
    &GLK::platform,
    &GLK::featureTable,
    &GlkHw1x3x6::gtSystemInfo,
};




// ------------------------------------------------------------------------------------------
// Broxton (GEN9)
// ------------------------------------------------------------------------------------------
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
    featureTable->flags.ftrSupportsOcl30 = true;
}
void BXT::setupHardwareInfoBase(const HardwareInfo* hwInfo) {
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->ThreadCount = gtSysInfo->EUCount * BXT::threadsPerEu;
    gtSysInfo->TotalVsThreads = 112;
    gtSysInfo->TotalHsThreads = 112;
    gtSysInfo->TotalDsThreads = 112;
    gtSysInfo->TotalGsThreads = 112;
    gtSysInfo->TotalPsThreadsWindowerRange = 64;
    gtSysInfo->CsrSizeInMb = 8;
    gtSysInfo->MaxEuPerSubSlice = BXT::maxEuPerSubSlice;
    gtSysInfo->MaxSlicesSupported = BXT::maxSlicesSupported;
    gtSysInfo->MaxSubSlicesSupported = BXT::maxSubSlicesSupported;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
    gtSysInfo->NumThreadsPerEu = BXT::threadsPerEu;
    setupFeatureAndWorkaroundTable(hwInfo);
}
SystemInfo BxtHw1x2x6::gtSystemInfo = {0};
void BxtHw1x2x6::setupHardwareInfo(const HardwareInfo* hwInfo) {
    setupHardwareInfoBase(hwInfo);
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->SliceCount = 1;
    gtSysInfo->L3CacheSizeInKb = 384;
    gtSysInfo->L3BankCount = 1;
    gtSysInfo->MaxFillRate = 8;
}
const HardwareInfo BxtHw1x2x6::hwInfo = {
    &BXT::platform,
    &BXT::featureTable,
    &BxtHw1x2x6::gtSystemInfo,
};
SystemInfo BxtHw1x3x6::gtSystemInfo = {0};
void BxtHw1x3x6::setupHardwareInfo(const HardwareInfo* hwInfo) {
    setupHardwareInfoBase(hwInfo);
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->SliceCount = 1;
    gtSysInfo->L3CacheSizeInKb = 384;
    gtSysInfo->L3BankCount = 1;
    gtSysInfo->MaxFillRate = 8;
}
const HardwareInfo BxtHw1x3x6::hwInfo = {
    &BXT::platform,
    &BXT::featureTable,
    &BxtHw1x3x6::gtSystemInfo,
};




// ------------------------------------------------------------------------------------------
// Broadwell (GEN8)
// ------------------------------------------------------------------------------------------
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
    featureTable->flags.ftrSupportsOcl21 = true;
    featureTable->flags.ftrSupportsOcl30 = true;
    featureTable->flags.ftrSupportsIndependentForwardProgress = true;
    featureTable->flags.ftrSupportsInteger64BitAtomics = true;
}
void BDW::setupHardwareInfoBase(const HardwareInfo* hwInfo) {
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->ThreadCount = gtSysInfo->EUCount * BDW::threadsPerEu;
    gtSysInfo->TotalVsThreads = 336;
    gtSysInfo->TotalHsThreads = 336;
    gtSysInfo->TotalDsThreads = 336;
    gtSysInfo->TotalGsThreads = 336;
    gtSysInfo->TotalPsThreadsWindowerRange = 64;
    gtSysInfo->CsrSizeInMb = 8;
    gtSysInfo->MaxEuPerSubSlice = BDW::maxEuPerSubSlice;
    gtSysInfo->MaxSlicesSupported = BDW::maxSlicesSupported;
    gtSysInfo->MaxSubSlicesSupported = BDW::maxSubSlicesSupported;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
    gtSysInfo->NumThreadsPerEu = BDW::threadsPerEu;
    setupFeatureAndWorkaroundTable(hwInfo);
}
SystemInfo BdwHw1x2x6::gtSystemInfo = {0};
void BdwHw1x2x6::setupHardwareInfo(const HardwareInfo* hwInfo) {
    setupHardwareInfoBase(hwInfo);
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->SliceCount = 1;
    gtSysInfo->L3CacheSizeInKb = 384;
    gtSysInfo->L3BankCount = 2;
    gtSysInfo->MaxFillRate = 8;
}
const HardwareInfo BdwHw1x2x6::hwInfo = {
    &BDW::platform,
    &BDW::featureTable,
    &BdwHw1x2x6::gtSystemInfo,
};
SystemInfo BdwHw1x3x6::gtSystemInfo = {0};
void BdwHw1x3x6::setupHardwareInfo(const HardwareInfo* hwInfo) {
    setupHardwareInfoBase(hwInfo);
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->SliceCount = 1;
    gtSysInfo->L3CacheSizeInKb = 768;
    gtSysInfo->L3BankCount = 4;
    gtSysInfo->MaxFillRate = 8;
}
const HardwareInfo BdwHw1x3x6::hwInfo = {
    &BDW::platform,
    &BDW::featureTable,
    &BdwHw1x3x6::gtSystemInfo,
};
SystemInfo BdwHw1x3x8::gtSystemInfo = {0};
void BdwHw1x3x8::setupHardwareInfo(const HardwareInfo* hwInfo) {
    setupHardwareInfoBase(hwInfo);
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->SliceCount = 1;
    gtSysInfo->L3CacheSizeInKb = 384;
    gtSysInfo->L3BankCount = 2;
    gtSysInfo->MaxFillRate = 8;
}
const HardwareInfo BdwHw1x3x8::hwInfo = {
    &BDW::platform,
    &BDW::featureTable,
    &BdwHw1x3x8::gtSystemInfo,
};
SystemInfo BdwHw2x3x8::gtSystemInfo = {0};
void BdwHw2x3x8::setupHardwareInfo(const HardwareInfo* hwInfo) {
    setupHardwareInfoBase(hwInfo);
    SystemInfo* gtSysInfo = hwInfo->gtSystemInfo;
    gtSysInfo->SliceCount = 2;
    gtSysInfo->L3CacheSizeInKb = 1536;
    gtSysInfo->L3BankCount = 8;
    gtSysInfo->MaxFillRate = 16;
}
const HardwareInfo BdwHw2x3x8::hwInfo = {
    &BDW::platform,
    &BDW::featureTable,
    &BdwHw2x3x8::gtSystemInfo,
};


const char* deviceExtensionsList = "cl_khr_byte_addressable_store "
                                   "cl_khr_fp16 "
                                   "cl_khr_global_int32_base_atomics "
                                   "cl_khr_global_int32_extended_atomics "
                                   "cl_khr_icd "
                                   "cl_khr_local_int32_base_atomics "
                                   "cl_khr_local_int32_extended_atomics "
                                   "cl_intel_command_queue_families "
                                   "cl_intel_subgroups "
                                   "cl_intel_required_subgroup_size "
                                   "cl_intel_subgroups_short "
                                   "cl_khr_spir "
                                   "cl_intel_accelerator "
                                   "cl_intel_driver_diagnostics "
                                   "cl_khr_priority_hints "
                                   "cl_khr_throttle_hints "
                                   "cl_khr_create_command_queue "
                                   "cl_intel_subgroups_char "
                                   "cl_intel_subgroups_long "
                                   "cl_khr_il_program "
                                   "cl_intel_mem_force_host_memory "
                                   "cl_khr_subgroup_extended_types "
                                   "cl_khr_subgroup_non_uniform_vote "
                                   "cl_khr_subgroup_ballot "
                                   "cl_khr_subgroup_non_uniform_arithmetic "
                                   "cl_khr_subgroup_shuffle "
                                   "cl_khr_subgroup_shuffle_relative "
                                   "cl_khr_subgroup_clustered_reduce "
                                   "cl_intel_device_attribute_query "
                                   "cl_khr_suggested_local_work_size ";



