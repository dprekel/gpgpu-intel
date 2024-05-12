#pragma once

#include <stdio.h>
#include <stdint.h>
#include <memory>
#include <vector>

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

enum SECTION_HEADER_TYPE : uint32_t {
    SHT_NULL = 0,                           // inactive section header
    SHT_PROGBITS = 1,                       // program data
    SHT_SYMTAB = 2,                         // symbol table
    SHT_STRTAB = 3,                         // string table
    SHT_RELA = 4,                           // relocation entries with add
    SHT_HASH = 5,                           // symbol hash table
    SHT_DYNAMIC = 6,                        // dynamic linking info
    SHT_NOTE = 7,                           // notes
    SHT_NOBITS = 8,                         // program "no data" space (bss)
    SHT_REL = 9,                            // relocation entries (without add)
    SHT_SHLIB = 10,                         // reserved
    SHT_DYNSYM = 11,                        // dynamic linker symbol table
    SHT_INIT_ARRAY = 14,                    // array of constructors
    SHT_FINI_ARRAY = 15,                    // array of destructors
    SHT_PREINIT_ARRAY = 16,                 // aaray of pre-constructors
    SHT_GROUP = 17,                         // section group
    SHT_SYMTAB_SHNDX = 18,                  // extended section indices
    SHT_NUM = 19,                           // number of defined types
    SHT_LOOS = 0x60000000,                  // start of os-specifc
    SHT_OPENCL_RESERVED_START = 0xff000000, // start of Intel OCL SHT_TYPES
    SHT_OPENCL_RESERVED_END = 0xff00000c    // end of Intel OCL SHT_TYPES
};

enum SHT_OPENCL : uint32_t {
    SHT_OPENCL_DEV_BINARY = 0xff000005,
    SHT_OPENCL_OPTIONS = 0xff000006,
    SHT_OPENCL_SPIRV = 0xff000009
};

struct ElfSectionHeader {
    uint32_t name        = 0u;
    uint32_t type        = 0u; //SHT_NULL;
    uint64_t flags       = 0u; //SHF_NONE;
    uint64_t addr        = 0u;
    uint64_t offset      = 0u;
    uint64_t size        = 0u;
    uint32_t link        = 0u; //SHN_UNDEF;
    uint32_t info        = 0u;
    uint64_t addralign   = 0u;
    uint64_t entsize     = 0u;
};

struct ElfProgramHeader {
    uint32_t type        = 0u; //PT_NULL;
    uint32_t flags       = 0u; //PF_NONE;
    uint64_t offset      = 0u;
    uint64_t vAddr       = 0u;
    uint64_t pAddr       = 0u;
    uint64_t fileSz      = 0u;
    uint64_t memSz       = 0u;
    uint64_t align       = 1u;
};

struct ElfFileHeader {
    char magic[4]        = {0x7f, 'E', 'L', 'F'};
    uint8_t eClass       = 0u; //EI_CLASS_NONE;
    uint8_t data         = 1u; //EI_DATA_LITTLE_ENDIAN;
    uint8_t Version      = 1u; //EV_CURRENT;
    uint8_t osAbi        = 0u;
    uint8_t abiVersion   = 0u;
    char padding[7]      = {};

    uint16_t type        = 0u; //ET_NONE;
    uint16_t machine     = 0u; //EM_NONE;
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
    uint16_t shStrNdx    = 0u; //SHN_UNDEF;
};


struct DataStruct {
    const char* data;
    size_t dataLength;
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
    void appendSection(uint32_t sectionType, const char* sectionLabel, std::vector<ElfSectionHeader>& sectionHeaders, DataStruct& sectionData, std::vector<uint8_t>& data, std::vector<char>& stringTable);
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
    DataStruct options;    
    DataStruct sourceCode;
    DataStruct intermediateRepresentation;
    DataStruct deviceBinary;

    uint64_t srcType;
    uint64_t intermediateType;
    uint64_t outType;

    const uint8_t* header;
    const uint8_t* patchListBlob;
    const uint8_t* kernelInfoBlob;
    KernelFromPatchtokens kernelData;

    uint64_t defaultDataAlignment;
};




