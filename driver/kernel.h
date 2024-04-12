#pragma once

#include "igc_interface.h"
#include "hwinfo.h"
#include "gpgpu.h"

struct ProgramBinaryHeader {
    uint32_t Magic;
    uint32_t Version;
    uint32_t Device;
    uint32_t GPUPointerSizeInBytes;
    uint32_t NumberOfKernels;
    uint32_t SteppingId;
    uint32_t PatchListSize;
};

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
    int createMetadata();
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

    const char* deviceBinary;
};
