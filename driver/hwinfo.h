#pragma once

#include <stdint.h>

#include "hwinfo_structs.h"


// Ponte Vecchio
struct PVC {
    static Platform platform;
    static FeatureTable featureTable;
    static const uint32_t threadsPerEu = 8;
    static const uint32_t maxEuPerSubSlice = 8;
    static const uint32_t maxSlicesSupported = 8;
    static const uint32_t maxSubSlicesSupported = 64;
    static const uint32_t maxDualSubSlicesSupported = 64;
    static void setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo);
};
class PvcHwConfig : public PVC {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};




// DG1
struct DG1 {
    static Platform platform;
    static FeatureTable featureTable;
    static const uint32_t threadsPerEu = 7;
    static const uint32_t maxEuPerSubSlice = 16;
    static const uint32_t maxSlicesSupported = 1;
    static const uint32_t maxSubSlicesSupported = 6;
    static const uint32_t maxDualSubSlicesSupported = 6;
    static void setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo);
};
class Dg1HwConfig : public DG1 {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};




// Alchemist
struct DG2 {
    static Platform platform;
    static FeatureTable featureTable;
    static const uint32_t threadsPerEu = 8;
    static const uint32_t maxEuPerSubSlice = 16;
    static const uint32_t maxSlicesSupported = 8;
    static const uint32_t maxSubSlicesSupported = 32;
    static const uint32_t maxDualSubSlicesSupported = 32;
    static void setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo);
};
class Dg2HwConfig : public DG2 {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};




// Meteorlake
struct MTL {
    static Platform platform;
    static FeatureTable featureTable;
    static const uint32_t threadsPerEu = 8;
    static const uint32_t maxEuPerSubSlice = 16;
    static const uint32_t maxSlicesSupported = 8;
    static const uint32_t maxSubSlicesSupported = 32;
    static const uint32_t maxDualSubSlicesSupported = 32;
    static void setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo);
};
class MtlHwConfig : public MTL {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};




// Arrowlake
struct ARL {
    static Platform platform;
    static FeatureTable featureTable;
    static const uint32_t threadsPerEu = 8;
    static void setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo);
};
class ArlHwConfig : public ARL {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};




// Rocketlake (GEN12LP)
struct RKL {
    static Platform platform;
    static FeatureTable featureTable;
    static const uint32_t threadsPerEu = 7;
    static const uint32_t maxEuPerSubSlice = 16;
    static const uint32_t maxSlicesSupported = 1;
    static void setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo);
};
class RklHwConfig : public RKL {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};




// AlderlakeS (GEN12LP)
struct ADLS {
    static Platform platform;
    static FeatureTable featureTable;
    static const uint32_t threadsPerEu = 7;
    static const uint32_t maxEuPerSubSlice = 8;
    static const uint32_t maxSlicesSupported = 1;
    static const uint32_t maxSubSlicesSupported = 8;
    static void setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo);
};
class AdlsHwConfig : public ADLS {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};




// AlderlakeN (GEN12LP)
struct ADLN {
    static Platform platform;
    static FeatureTable featureTable;
    static const uint32_t threadsPerEu = 7;
    static const uint32_t maxEuPerSubSlice = 16;
    static const uint32_t maxSlicesSupported = 1;
    static const uint32_t maxSubSlicesSupported = 6;
    static const uint32_t maxDualSubSlicesSupported = 12;
    static void setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo);
};
class AdlnHwConfig : public ADLN {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};




// AlderlakeP (GEN12LP)
struct ADLP {
    static Platform platform;
    static FeatureTable featureTable;
    static const uint32_t threadsPerEu = 7;
    static const uint32_t maxEuPerSubSlice = 16;
    static const uint32_t maxSlicesSupported = 1;
    static const uint32_t maxSubSlicesSupported = 6;
    static const uint32_t maxDualSubSlicesSupported = 12;
    static void setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo);
};
class AdlpHwConfig : public ADLP {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};




// Tigerlake
struct TGLLP {
    static Platform platform;
    static FeatureTable featureTable;
    static const uint32_t threadsPerEu = 7;
    static const uint32_t maxEuPerSubSlice = 16;
    static const uint32_t maxSlicesSupported = 1;
    static const uint32_t maxSubSlicesSupported = 6;
    static const uint32_t maxDualSubSlicesSupported = 6;
    static void setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo);
};
class TgllpHw1x2x16: public TGLLP {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class TgllpHw1x6x16: public TGLLP {
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
    static const uint32_t maxSubSlicesSupported = 8;
    static void setupHardwareInfoBase(const HardwareInfo* hwInfo);
    static void setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo);
};
class IcllpHw1x8x8 : public ICLLP {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class IcllpHw1x4x8 : public ICLLP {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class IcllpHw1x6x8 : public ICLLP {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};




// Lakefield
struct LKF {
    static Platform platform;
    static FeatureTable featureTable;
    static const uint32_t threadsPerEu = 7;
    static const uint32_t maxEuPerSubSlice = 8;
    static const uint32_t maxSlicesSupported = 1;
    static const uint32_t maxSubSlicesSupported = 8;
    static void setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo);
};
class LkfHw1x8x8 : public LKF {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};




// Elkhartlake
struct EHL {
    static Platform platform;
    static FeatureTable featureTable;
    static const uint32_t threadsPerEu = 7;
    static const uint32_t maxEuPerSubSlice = 8;
    static const uint32_t maxSlicesSupported = 1;
    static const uint32_t maxSubSlicesSupported = 4;
    static void setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo);
};
class EhlHwConfig : public EHL {
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
    static const uint32_t maxSubSlicesSupported = 9;
    static void setupHardwareInfoBase(const HardwareInfo* hwInfo);
    static void setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo);
};
class SklHw1x2x6 : public SKL {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class SklHw1x3x6 : public SKL {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class SklHw1x3x8 : public SKL {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class SklHw2x3x8 : public SKL {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class SklHw3x3x8 : public SKL {
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
    static const uint32_t maxEuPerSubSlice = 8;
    static const uint32_t maxSlicesSupported = 3;
    static const uint32_t maxSubSlicesSupported = 9;
    static void setupHardwareInfoBase(const HardwareInfo* hwInfo);
    static void setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo);
};
class KblHw1x2x6 : public KBL {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class KblHw1x3x6 : public KBL {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class KblHw1x3x8 : public KBL {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class KblHw2x3x8 : public KBL {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class KblHw3x3x8 : public KBL {
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
    static const uint32_t maxEuPerSubSlice = 8;
    static const uint32_t maxSlicesSupported = 3;
    static const uint32_t maxSubSlicesSupported = 9;
    static void setupHardwareInfoBase(const HardwareInfo* hwInfo);
    static void setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo);
};
class CflHw1x2x6 : public CFL {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class CflHw1x3x6 : public CFL {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class CflHw1x3x8 : public CFL {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class CflHw2x3x8 : public CFL {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class CflHw3x3x8 : public CFL {
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
    static const uint32_t maxEuPerSubSlice = 6;
    static const uint32_t maxSlicesSupported = 1;
    static const uint32_t maxSubSlicesSupported = 3;
    static void setupHardwareInfoBase(const HardwareInfo* hwInfo);
    static void setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo);
};
class GlkHw1x2x6 : public GLK {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class GlkHw1x3x6 : public GLK {
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
    static const uint32_t maxEuPerSubSlice = 6;
    static const uint32_t maxSlicesSupported = 1;
    static const uint32_t maxSubSlicesSupported = 3;
    static void setupHardwareInfoBase(const HardwareInfo* hwInfo);
    static void setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo);
};
class BxtHw1x2x6 : public BXT {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class BxtHw1x3x6 : public BXT {
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
    static const uint32_t maxEuPerSubSlice = 8;
    static const uint32_t maxSlicesSupported = 2;
    static const uint32_t maxSubSlicesSupported = 6;
    static void setupHardwareInfoBase(const HardwareInfo* hwInfo);
    static void setupFeatureAndWorkaroundTable(const HardwareInfo* hwInfo);
};
class BdwHw1x2x6 : public BDW {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class BdwHw1x3x6 : public BDW {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class BdwHw1x3x8 : public BDW {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};
class BdwHw2x3x8 : public BDW {
  public:
    static void setupHardwareInfo(const HardwareInfo* hwInfo);
    static const HardwareInfo hwInfo;
  private:
    static SystemInfo gtSystemInfo;
};


