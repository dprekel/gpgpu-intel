#pragma once

#include "igc_interface.h"
#include "hwinfo.h"
#include "gpgpu.h"

#pragma pack( push, 1 )

enum PATCH_TOKEN {
    PATCH_TOKEN_SAMPLER_STATE_ARRAY,
    PATCH_TOKEN_BINDING_TABLE_STATE,
    PATCH_TOKEN_MEDIA_INTERFACE_DESCRIPTOR_LOAD,
    PATCH_TOKEN_INTERFACE_DESCRIPTOR_DATA
};

struct ProgramBinaryHeader {
    uint32_t Magic;
    uint32_t Version;
    uint32_t Device;
    uint32_t GPUPointerSizeInBytes;
    uint32_t NumberOfKernels;
    uint32_t SteppingId;
    uint32_t PatchListSize;
};

struct KernelBinaryHeader {
    uint32_t CheckSum;
    uint64_t ShaderHashCode;
    uint32_t KernelNameSize;
    uint32_t PatchListSize;
    uint32_t KernelHeapSize;
    uint32_t GeneralStateHeapSize;
    uint32_t DynamicStateHeapSize;
    uint32_t SurfaceStateHeapSize;
    uint32_t KernelUnpaddedSize;
};

struct PatchItemHeader {
    uint32_t Token;
    uint32_t Size;
};

struct KernelFromPatchtokens {
    const KernelBinaryHeader* header;
    const uint8_t* name;
    const uint8_t* isa;
    const uint8_t* generalState;
    const uint8_t* dynamicState;
    const uint8_t* surfaceState;
    const uint8_t* kernelInfo;
    const uint8_t* patchList;
    const PatchSamplerStateArray* samplerStateArray;
    const PatchBindingTableState* bindingTableState;
    const PatchMediaInterfaceDescriptorLoad* mediaInterfaceDescriptorLoad;
    const PatchInterfaceDescriptorData* interfaceDescriptorData;
};

#pragma pack ( pop )

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
    const void* ptrOffset(const void* ptrBefore, size_t offset);
    int extractMetadata();
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
    const uint8_t* header;
    const uint8_t* patchListBlob;
    const uint8_t* kernelInfoBlob;
    KernelFromPatchtokens* kernelData;
};





