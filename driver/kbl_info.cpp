#include "kbl_info.h"
#include "hwinfo.h"

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

FeatureTable KBL::featureTable = {};
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


