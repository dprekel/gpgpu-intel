#pragma once

#include "igc_interface.h"
#include "hwinfo.h"
#include "gpgpu.h"

class Kernel {
  public:
    Kernel(GPU* gpuInfo, const char* filename, const char* options);
    ~Kernel();
    int loadProgramSource();
    ICIF* CreateInterface(CIFMain* cifMain, uint64_t interfaceID, uint64_t interfaceVersion);
    IgcBuffer* CreateIgcBuffer(CIFMain* cifMain, const char* data, size_t size);
    int loadCompiler(const char* libName, CIFMain** cifMain);
    FclOclTranslationCtx* createFclTranslationCtx();
    void TransferPlatformInfo(PlatformInfo* igcPlatform, Platform* platform);
    void TransferSystemInfo(GTSystemInfo* igcGetSystemInfo, SystemInfo* gtSystemInfo);
    void TransferFeaturesInfo(IgcFeaturesAndWorkarounds* igcFeWa, FeatureTable* featureTable);
    IgcOclTranslationCtx* createIgcTranslationCtx();
    int build();
    int createKernelAllocation();
  private:
    GPU* gpuInfo;
    const char* filename;
    const char* options;
    size_t optionsSize;

    const char* igcName;
    const char* fclName;
    CIFMain* igcMain;
    CIFMain* fclMain;
    const char* srcCode;
    size_t srcSize;
    uint64_t srcType;
    uint64_t intermediateType;
    uint64_t outType;
};
