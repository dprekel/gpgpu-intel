#pragma once

#include <stdio.h>
#include <stdint.h>
#include <memory>

#include "device.h"
#include "igc_interface.h"
#include "hwinfo.h"
#include "gpgpu.h"

class Device;
class Context;
struct DeviceDescriptor;

#pragma pack( push, 1 )

enum PATCH_TOKEN {
    PATCH_TOKEN_SAMPLER_STATE_ARRAY = 5,
    PATCH_TOKEN_BINDING_TABLE_STATE = 8,
    PATCH_TOKEN_MEDIA_INTERFACE_DESCRIPTOR_LOAD = 19,
    PATCH_TOKEN_INTERFACE_DESCRIPTOR_DATA = 21,
    PATCH_TOKEN_EXECUTION_ENVIRONMENT = 23,
    PATCH_TOKEN_KERNEL_ATTRIBUTES_INFO = 27
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

struct PatchSamplerStateArray : PatchItemHeader {
    uint32_t Offset;
    uint32_t Count;
    uint32_t BorderColorOffset;
};

struct PatchBindingTableState : PatchItemHeader {
    uint32_t Offset;
    uint32_t Count;
    uint32_t SurfaceStateOffset;
};

struct PatchMediaInterfaceDescriptorLoad : PatchItemHeader {
    uint32_t InterfaceDescriptorDataOffset;
};

struct PatchInterfaceDescriptorData : PatchItemHeader {
    uint32_t Offset;
    uint32_t SamplerStateOffset;
    uint32_t KernelOffset;
    uint32_t BindingTableOffset;
};

struct PatchExecutionEnvironment : PatchItemHeader {
    uint32_t RequiredWorkGroupSizeX;
    uint32_t RequiredWorkGroupSizeY;
    uint32_t RequiredWorkGroupSizeZ;
    uint32_t LargestCompiledSIMDSize;
    uint32_t CompiledSubGroupsNumber;
    uint32_t HasBarriers;
    uint32_t DisableMidThreadPreemption;
    uint32_t CompiledSIMD8;
    uint32_t CompiledSIMD16;
    uint32_t CompiledSIMD32;
    uint32_t HasDeviceEnqueue;
    uint32_t MayAccessUndeclaredResource;
    uint32_t UsesFencesForReadWriteImages;
    uint32_t UsesStatelessSpillFill;
    uint32_t UsesMultiScratchSpaces;
    uint32_t IsCoherent;
    uint32_t IsInitializer;
    uint32_t IsFinalizer;
    uint32_t SubgroupIndependentForwardProgressRequired;
    uint32_t CompiledForGreaterThan4GBBuffers;
    uint32_t NumGRFRequired;
    uint32_t WorkgroupWalkOrderDims;
    uint32_t HasGlobalAtomics;
    uint32_t HasDPAS;
    uint32_t reserved1;
    uint32_t reserved2;
    uint32_t StatelessWritesCount;
    uint32_t IndirectStatelessCount;
    uint32_t UseBindlessMode;
    uint32_t HasStackCalls;
    uint64_t SIMDInfo;
};

struct PatchKernelAttributesInfo : PatchItemHeader {
    uint32_t AttributesSize;
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
    const uint8_t* patchListEnd;
    const PatchSamplerStateArray* samplerStateArray = nullptr;
    const PatchBindingTableState* bindingTableState = nullptr;
    const PatchMediaInterfaceDescriptorLoad* mediaInterfaceDescriptorLoad = nullptr;
    const PatchInterfaceDescriptorData* interfaceDescriptorData = nullptr;
    const PatchExecutionEnvironment* executionEnvironment = nullptr;
    const PatchKernelAttributesInfo* kernelAttributesInfo = nullptr;
};

#pragma pack ( pop )

struct ElfFileHeader {
    char magic[4]        = {0x7f, 'E', 'L', 'F'};
    uint8_t eClass       = EI_CLASS_NONE;
    uint8_t data         = EI_CLASS_LITTLE_ENDIAN;
    uint8_t version      = EV_CURRENT;
    uint8_t osAbi        = 0u;
    uint8_t abiVersion   = 0u;
    char padding[7]      = {};

    uint16_t type        = ET_NONE;
    uint16_t machine     = EM_NONE;
    uint32_t version     = 1u;
    uint64_t entry       = 0u;
    uint64_t phOff       = 0u;
    uint64_t shOff       = 0u;
    uint32_t flags       = 0u;
    uint16_t ehSize      = sizeof(ElfFileHeader);
    uint16_t phEntSize   = sizeof(ElfProgramHeader);
    uint16_t phNum       = 0u;
    uint16_t shEntSize   = sizeof(ElfSectionHeader);
    uint16_t shNum       = 0u;
    uint16_t shStrNdx    = SHN_UNDEF;
};

struct ElfSectionHeader {
    uint32_t name        = 0u;
    uint32_t type        = SHT_NULL;
    uint64_t flags       = SHF_NONE;
    uint64_t addr        = 0u;
    uint64_t offset      = 0u;
    uint64_t size        = 0u;
    uint32_t link        = SHN_UNDEF;
    uint32_t info        = 0u;
    uint64_t addralign   = 0u;
    uint64_t entsize     = 0u;
};

struct ElfProgramHeader {
    uint32_t type        = PT_NULL;
    uint32_t flags       = PF_NONE;
    uint64_t offset      = 0u;
    uint64_t vAddr       = 0u;
    uint64_t pAddr       = 0u;
    uint64_t fileSz      = 0u;
    uint64_t memSz       = 0u;
    uint64_t align       = 1u;
};


class Kernel : public pKernel {
  public:
    Kernel(Context* context, const char* filename, const char* options);
    ~Kernel();
    KernelFromPatchtokens* getKernelData();
    int initialize();
    int loadProgramSource();
    int build(uint16_t chipset_id);
    ICIF* CreateInterface(CIFMain* cifMain, uint64_t interfaceID, uint64_t interfaceVersion);
    IgcBuffer* CreateIgcBuffer(CIFMain* cifMain, const char* data, size_t size);
    int loadCompiler(const char* libName, CIFMain** cifMain);
    FclOclTranslationCtx* createFclTranslationCtx();
    void TransferPlatformInfo(PlatformInfo* igcPlatform, Platform* platform);
    void TransferSystemInfo(GTSystemInfo* igcGetSystemInfo, SystemInfo* gtSystemInfo);
    void TransferFeaturesInfo(IgcFeaturesAndWorkarounds* igcFeWa, FeatureTable* featureTable);
    IgcOclTranslationCtx* createIgcTranslationCtx();
    void decodeToken(const PatchItemHeader* token, KernelFromPatchtokens* kernelData);
    int disassembleBinary();
    std::vector<uint8_t> packDeviceBinary();
    int extractMetadata();
    int createSipKernel();

    Context* context;
  private:
    std::unique_ptr<DeviceDescriptor> descriptor;
    const char* igcName;
    const char* fclName;
    CIFMain* igcMain;
    CIFMain* fclMain;

    const char* filename;
    const char* options;
    size_t optionsSize;
    
    const char* srcCode;
    uint64_t srcSize;
    uint64_t srcType;
    uint64_t intermediateType;
    uint64_t outType;

    const char* deviceBinary;
    const uint8_t* header;
    const uint8_t* patchListBlob;
    const uint8_t* kernelInfoBlob;
    KernelFromPatchtokens kernelData;
};




