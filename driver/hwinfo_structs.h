#pragma once

#include <stdint.h>

struct Platform {
    uint64_t eProductFamily;
    uint64_t ePCHProductFamily;
    uint64_t eDisplayCoreFamily;
    uint64_t eRenderCoreFamily;
    uint64_t ePlatformType;
    uint16_t usDeviceID;
    uint16_t usRevId;
    uint16_t usDeviceID_PCH;
    uint16_t usRevId_PCH;
    uint64_t eGTType;
};

struct SystemInfo {
    uint32_t EUCount;
    uint32_t ThreadCount;
    uint32_t SliceCount;
    uint32_t SubSliceCount;
    uint32_t DualSubSliceCount;
    uint64_t L3CacheSizeInKb;
    uint64_t LLCCacheSizeInKb;
    uint64_t EdramSizeInKb;
    uint32_t L3BankCount;
    uint32_t MaxFillRate;
    uint32_t EuCountPerPoolMax;
    uint32_t EuCountPerPoolMin;
    uint32_t TotalVsThreads;
    uint32_t TotalHsThreads;
    uint32_t TotalDsThreads;
    uint32_t TotalGsThreads;
    uint32_t TotalPsThreadsWindowerRange;
    uint32_t TotalVsThreads_Pocs;
    uint32_t CsrSizeInMb;
    uint32_t MaxEuPerSubSlice;
    uint32_t MaxSlicesSupported;
    uint32_t MaxSubSlicesSupported;
    uint32_t MaxDualSubSlicesSupported;
    bool IsL3HashModeEnabled;
    //GT_VDBOX_INFO VDBoxInfo;
    //GT_VEBOX_INFO VEBoxInfo;
    //GT_SLICE_INFO SliceInfo[4];
    bool IsDynamicallyPopulated;
    //GT_SQIDI_INFO SquidiInfo;
    uint32_t ReservedCCSWays;
    //GT_CCS_INFO CCSInfo;
    //GT_MULTI_TILE_ARCH_INFO MultiTileArchInfo;
    uint32_t NumThreadsPerEu;
    //GT_CACHE_TYPES CacheTypes;
    uint32_t MaxVECS;
    uint32_t MemoryTypes;
};

struct FeatureTable {
    struct Flags {
        // DW0
        uint32_t ftrDesktop : 1;
        uint32_t ftrChannelSwizzlingXOREnabled : 1;
        uint32_t ftrGtBigDie : 1;
        uint32_t ftrGtMediumDie : 1;
        uint32_t ftrGtSmallDie : 1;
        uint32_t ftrGT1 : 1;
        uint32_t ftrGT1_5 : 1;
        uint32_t ftrGT2 : 1;
        uint32_t ftrGT2_5 : 1;
        uint32_t ftrGT3 : 1;
        uint32_t ftrGT4 : 1;
        uint32_t ftrIVBM0M1Platform : 1;
        uint32_t ftrSGTPVSKUStrapPresent : 1;
        uint32_t ftrGTA : 1;
        uint32_t ftrGTC : 1;
        uint32_t ftrGTX : 1;
        uint32_t ftr5Slice : 1;
        uint32_t ftrGpGpuMidBatchPreempt : 1;
        uint32_t ftrGpGpuThreadGroupLevelPreempt : 1;
        uint32_t ftrGpGpuMidThreadLevelPreempt : 1;
        uint32_t ftrIoMmuPageFaulting : 1;
        uint32_t ftrWddm2Svm : 1;
        uint32_t ftrPooledEuEnabled : 1;
        uint32_t ftrResourceStreamer : 1;
        uint32_t ftrPPGTT : 1;
        uint32_t ftrSVM : 1;
        uint32_t ftrEDram : 1;
        uint32_t ftrL3IACoherency : 1;
        uint32_t ftrIA32eGfxPTEs : 1;
        uint32_t ftr3dMidBatchPreempt : 1;
        uint32_t ftr3dObjectLevelPreempt : 1;
        uint32_t ftrPerCtxtPreemptionGranularityControl : 1;
        // DW1
        uint32_t ftrTileY : 1;
        uint32_t ftrDisplayYTiling : 1;
        uint32_t ftrTranslationTable : 1;
        uint32_t ftrUserModeTranslationTable : 1;
        uint32_t ftrEnableGuC : 1;
        uint32_t ftrFbc : 1;
        uint32_t ftrFbc2AddressTranslation : 1;
        uint32_t ftrFbcBlitterTracking : 1;
        uint32_t ftrFbcCpuTracking : 1;
        uint32_t ftrVcs2 : 1;
        uint32_t ftrVEBOX : 1;
        uint32_t ftrSingleVeboxSlice : 1;
        uint32_t ftrULT : 1;
        uint32_t ftrLCIA : 1;
        uint32_t ftrGttCacheInvalidation : 1;
        uint32_t ftrTileMappedResource : 1;
        uint32_t ftrAstcHdr2D : 1;
        uint32_t ftrAstcLdr2D : 1;
        uint32_t ftrStandardMipTailFormat : 1;
        uint32_t ftrFrameBufferLLC : 1;
        uint32_t ftrCrystalwell : 1;
        uint32_t ftrLLCBypass : 1;
        uint32_t ftrDisplayEngineS3d : 1;
        uint32_t ftrVERing : 1;
        uint32_t ftrWddm2GpuMmu : 1;
        uint32_t ftrWddm2_1_64kbPages : 1;
        uint32_t ftrWddmHwQueues : 1;
        uint32_t ftrMemTypeMocsDeferPAT : 1;
        uint32_t ftrKmdDaf : 1;
        uint32_t ftrSimulationMode : 1;
        uint32_t ftrE2ECompression : 1;
        uint32_t ftrLinearCCS : 1;
        //DW2
        uint32_t ftrCCSRing : 1;
        uint32_t ftrCCSNode : 1;
        uint32_t ftrRcsNode : 1;
        uint32_t ftrLocalMemory : 1;
        uint32_t ftrLocalMemoryAllows4KB : 1;
        uint32_t ftrFlatPhysCCS : 1;
        uint32_t ftrMultiTileArch : 1;
        uint32_t ftrCCSMultiInstance : 1;
        uint32_t ftrPpgtt64KBWalkOptimization : 1;
        uint32_t ftrUnified3DMediaCompressionFormats : 1;
        uint32_t reserved : 22;
    };

    union {
        Flags flags;
    };
};

struct HardwareInfo {
    Platform* platform;
    FeatureTable* featureTable;
    SystemInfo* gtSystemInfo;
};

enum PRODUCT_FAMILY {
    IGFX_UNKNOWN = 0,
    IGFX_BROADWELL = 16,
    IGFX_CHERRYVIEW,
    IGFX_SKYLAKE,
    IGFX_KABYLAKE,
    IGFX_COFFEELAKE,
    IGFX_WILLOWVIEW,
    IGFX_BROXTON,
    IGFX_GEMINILAKE,
    IGFX_CANNONLAKE,
    IGFX_ICELAKE,
    IGFX_ICELAKE_LP,
    IGFX_LAKEFIELD,
    IGFX_JASPERLAKE,
    IGFX_ELKHARTLAKE = 28,
    IGFX_TIGERLAKE_LP,
    IGFX_ROCKETLAKE,
    IGFX_ALDERLAKE_S,
    IGFX_ALDERLAKE_P,
    IGFX_ALDERLAKE_N,

    IGFX_DG1 = 1210,
    IGFX_XE_HP_SDV = 1250,
    IGFX_DG2 = 1270,
    IGFX_PVC = 1271,
    IGFX_METEORLAKE = 1272,
    IGFX_ARROWLAKE = 1273,
    IGFX_BMG = 1274,
    IGFX_LUNARLAKE = 1275,
    IGFX_MAX_PRODUCT,

    IGFX_GENNEXT               = 0x7ffffffe,
    PRODUCT_FAMILY_FORCE_ULONG = 0x7fffffff
};

enum GTTYPE {
    GTTYPE_GT1,
    GTTYPE_GT2,
    GTTYPE_GT2_FUSED_TO_GT1,
    GTTYPE_GT2_FUSED_TO_GT1_6,
    GTTYPE_GTL,
    GTTYPE_GTM,
    GTTYPE_GTH,
    GTTYPE_GT1_5,
    GTTYPE_GT1_75,
    GTTYPE_GT3,
    GTTYPE_GT4,
    GTTYPE_GT0,
    GTTYPE_GTA,
    GTTYPE_GTC,
    GTTYPE_GTX,
    GTTYPE_GT2_5,
    GTTYPE_GT3_5,
    GTTYPE_GT0_5,
    GTTYPE_UNDEFINED
};

enum PLATFORM_TYPE {
    PLATFORM_NONE,
    PLATFORM_DESKTOP,
    PLATFORM_MOBILE,
    PLATFORM_TABLET,
    PLATFORM_ALL = 255
};

enum GFX_CORE_FAMILY {
    IGFX_UNKNOWN_CORE,
    IGFX_GEN3_CORE,
    IGFX_GEN3_5_CORE,
    IGFX_GEN4_CORE,
    IGFX_GEN4_5_CORE,
    IGFX_GEN5_CORE,
    IGFX_GEN5_5_CORE,
    IGFX_GEN5_75_CORE,
    IGFX_GEN6_CORE,
    IGFX_GEN7_CORE,
    IGFX_GEN7_5_CORE,
    IGFX_GEN8_CORE,
    IGFX_GEN9_CORE,
    IGFX_GEN10_CORE,
    IGFX_GEN10LP_CORE,
    IGFX_GEN11_CORE,
    IGFX_GEN11LP_CORE,
    IGFX_GEN12_CORE,
    IGFX_GEN12LP_CORE,
    IGFX_XE_HP_CORE = 3077,
    IGFX_XE_HPG_CORE = 3079,
    IGFX_XE_HPC_CORE,
    IGFX_MAX_CORE,
    IGFX_GENNEXT_CORE = 2147483646,
    GFXCORE_FAMILY_FORCE_ULONG
};

enum PCH_PRODUCT_FAMILY {
    PCH_UNKNOWN,
    PCH_IBX,
    PCH_CPT,
    PCH_CPTR,
    PCH_PPT,
    PCH_LPT,
    PCH_LPTR,
    PCH_WPT,
    PCH_SPT,
    PCH_KBP,
    PCH_CNP_LP,
    PCH_CNP_H,
    PCH_ICP_LP,
    PCH_ICP_N,
    PCH_LKF,
    PCH_TGL_LP,
    PCH_CMP_LP,
    PCH_CMP_H,
    PCH_CMP_V,
    PCH_JSP_N,
    PCH_ADL_S,
    PCH_ADL_P,
    PCH_TGL_H,
    PCH_PRODUCT_FAMILY_FORCE_ULONG = 2147483647
};


