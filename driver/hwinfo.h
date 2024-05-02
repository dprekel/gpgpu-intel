#pragma once

#include <stdint.h>

#include "hwinfo.h"

// AlderlakeS (GEN12LP)
struct ADLS {
    static Platform platform;
    static FeatureTable featureTable;
    static const uint32_t threadsPerEu = 7;
    static const uint32_t maxEuPerSubSlice = 8;
    static const uint32_t maxSlicesSupported = 1;
    static const uint32_t maxSubslicesSupported = 8;
    static void setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo);
};
class ADLS_HW_CONFIG : public ADLS {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};




// Icelake (GEN11)
struct ICLLP {
    static Platform platform;
    static FeatureTable featureTable;
    static const uint32_t threadsPerEu = 7;
    static const uint32_t maxEuPerSubSlice = 8;
    static const uint32_t maxSlicesSupported = 1;
    static const uint32_t maxSubslicesSupported = 8;
    static void setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo);
};
class ICLLP_1x8x8 : public ICLLP {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class ICLLP_1x4x8 : public ICLLP {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class ICLLP_1x6x8 : public ICLLP {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};




struct LKF {
    static Platform platform;
    static FeatureTable featureTable;
    static const uint32_t threadsPerEu = 7;
    static const uint32_t maxEuPerSubSlice = 8;
    static const uint32_t maxSlicesSupported = 1;
    static const uint32_t maxSubslicesSupported = 8;
    static void setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo);
};
class LKF_1x8x8 : public LKF {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};



struct EHL {
    static Platform platform;
    static FeatureTable featureTable;
    static const uint32_t threadsPerEu = 7;
    static const uint32_t maxEuPerSubSlice = 8;
    static const uint32_t maxSlicesSupported = 1;
    static const uint32_t maxSubslicesSupported = 4;
    static void setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo);
};
class EHL_HW_CONFIG : public EHL {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};




// Skylake (GEN9)
struct SKL {
    static Platform platform;
    static FeatureTable featureTable;
    static const uint32_t threadsPerEu = 7;
    static const uint32_t maxEuPerSubSlice = 8;
    static const uint32_t maxSlicesSupported = 3;
    static const uint32_t maxSubslicesSupported = 9;
    static void setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo);
};
class SKL_1x2x6 : public SKL {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class SKL_1x3x6 : public SKL {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class SKL_1x3x8 : public SKL {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class SKL_2x3x8 : public SKL {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class SKL_3x3x8 : public SKL {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};




// Kabylake (GEN9)
struct KBL {
    static Platform platform;
    static FeatureTable featureTable;
    static const uint32_t threadsPerEu = 7;
    static const uint32_t maxEuPerSubslice = 8;
    static const uint32_t maxSlicesSupported = 3;
    static const uint32_t maxSubslicesSupported = 9;
    static void setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo);
};
class KBL_1x2x6 : public KBL {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class KBL_1x3x6 : public KBL {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class KBL_1x3x8 : public KBL {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class KBL_2x3x8 : public KBL {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class KBL_3x3x8 : public KBL {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};




// Coffeelake (GEN9)
struct CFL {
    static Platform platform;
    static FeatureTable featureTable;
    static const uint32_t threadsPerEu = 7;
    static const uint32_t maxEuPerSubslice = 8;
    static const uint32_t maxSlicesSupported = 3;
    static const uint32_t maxSubslicesSupported = 9;
    static void setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo);
};
class CFL_1x2x6 : public CFL {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class CFL_1x3x6 : public CFL {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class CFL_1x3x8 : public CFL {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class CFL_2x3x8 : public CFL {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class CFL_3x3x8 : public CFL {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};




// Geminilake (GEN9)
struct GLK {
    static Platform platform;
    static FeatureTable featureTable;
    static const uint32_t threadsPerEu = 6;
    static const uint32_t maxEuPerSubslice = 6;
    static const uint32_t maxSlicesSupported = 1;
    static const uint32_t maxSubslicesSupported = 3;
    static void setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo);
};
class GLK_1x2x6 : public GLK {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class GLK_1x3x6 : public GLK {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};




// Broxton (GEN9)
struct BXT {
    static Platform platform;
    static FeatureTable featureTable;
    static const uint32_t threadsPerEu = 6;
    static const uint32_t maxEuPerSubslice = 6;
    static const uint32_t maxSlicesSupported = 1;
    static const uint32_t maxSubslicesSupported = 3;
    static void setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo);
};
class BXT_1x2x6 : public BXT {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class BXT_1x3x6 : public BXT {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};




// Broadwell (GEN8)
struct BDW {
    static Platform platform;
    static FeatureTable featureTable;
    static const uint32_t threadsPerEu = 7;
    static const uint32_t maxEuPerSubslice = 8;
    static const uint32_t maxSlicesSupported = 2;
    static const uint32_t maxSubslicesSupported = 6;
    static void setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo);
};
class BDW_1x2x6 : public BDW {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class BDW_1x3x6 : public BDW {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class BDW_1x3x8 : public BDW {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class BDW_2x3x8 : public BDW {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};


