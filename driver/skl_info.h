#pragma once

#include <stdint.h>

#include "hwinfo.h"

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
