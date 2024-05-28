#pragma once

#include <stdint.h>
#include <stdio.h>

#include <memory>
#include <vector>

#include "device.h"
#include "gpgpu.h"
#include "hwinfo.h"
#include "igc_interface.h"

class Device;
class Context;
struct DeviceDescriptor;

enum PATCH_TOKEN {
    PATCH_TOKEN_SAMPLER_STATE_ARRAY = 5,
    PATCH_TOKEN_BINDING_TABLE_STATE = 8,
    PATCH_TOKEN_DATA_PARAMETER_BUFFER = 17,
    PATCH_TOKEN_MEDIA_VFE_STATE = 18,
    PATCH_TOKEN_MEDIA_INTERFACE_DESCRIPTOR_LOAD = 19,
    PATCH_TOKEN_INTERFACE_DESCRIPTOR_DATA = 21,
    PATCH_TOKEN_EXECUTION_ENVIRONMENT = 23,
    PATCH_TOKEN_KERNEL_ATTRIBUTES_INFO = 27,
    PATCH_TOKEN_MEDIA_VFE_STATE_SLOT1 = 55
};

enum DATA_PARAMETER {
    DATA_PARAMETER_KERNEL_ARGUMENT = 1,
    DATA_PARAMETER_BUFFER_STATEFUL = 43
};

#pragma pack( push, 1 )

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
struct PatchMediaVFEState : PatchItemHeader {
    uint32_t ScratchSpaceOffset;
    uint32_t PerThreadScratchSpace;
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
struct PatchDataParameterBuffer : PatchItemHeader {
    uint32_t Type;
    uint32_t ArgumentNumber;
    uint32_t Offset;
    uint32_t DataSize;
    uint32_t SourceOffset;
    uint32_t LocationIndex;
    uint32_t LocationIndex2;
    uint32_t IsEmulationArgument;
};
struct PatchStatelessGlobalMemoryObjectKernelArgument : PatchItemHeader {
    uint32_t ArgumentNumber;
    uint32_t SurfaceStateHeapOffset;
    uint32_t DataParamOffset;
    uint32_t DataParamSize;
    uint32_t LocationIndex;
    uint32_t LocationIndex2;
    uint32_t IsEmulationArgument;
};
struct PatchStatelessGlobalMemoryObjectKernelArgument : PatchItemHeader {
    uint32_t ArgumentNumber;
    uint32_t SurfaceStateHeapOffset;
    uint32_t DataParamOffset;
    uint32_t DataParamSize;
    uint32_t LocationIndex;
    uint32_t LocationIndex2;
    uint32_t IsEmulationArgument;
};
struct PatchStatelessConstantMemoryObjectKernelArgument : PatchItemHeader {
    uint32_t GlobalBufferIndex;
    uint32_t SurfaceStateHeapOffset;
    uint32_t DataParamOffset;
    uint32_t DataParamSize;
};


struct KernelFromPatchtokens {
    const KernelBinaryHeader* header = nullptr;
    const uint8_t* name = nullptr;
    const uint8_t* isa = nullptr;
    const uint8_t* generalState = nullptr;
    const uint8_t* dynamicState = nullptr;
    const uint8_t* surfaceState = nullptr;
    const uint8_t* kernelInfo = nullptr;
    const uint8_t* patchList = nullptr;
    const uint8_t* patchListEnd = nullptr;
    const PatchSamplerStateArray* samplerStateArray = nullptr;
    const PatchBindingTableState* bindingTableState = nullptr;
    const PatchMediaVFEState* mediaVfeState[2] = {nullptr, nullptr};
    const PatchMediaInterfaceDescriptorLoad* mediaInterfaceDescriptorLoad = nullptr;
    const PatchInterfaceDescriptorData* interfaceDescriptorData = nullptr;
    const PatchExecutionEnvironment* executionEnvironment = nullptr;
    const PatchKernelAttributesInfo* kernelAttributesInfo = nullptr;
};

#pragma pack ( pop )


struct ArgDescPointer {
    uint16_t bindful        = 0;
    uint16_t stateless      = 0;
    uint16_t bindless       = 0;
    uint16_t bufferOffset   = 0;
};

struct DataStruct {
    const char* data = nullptr;
    size_t dataLength;
};


class Kernel : public pKernel {
  public:
    typedef uint32_t (Kernel::*KernelArgHandler)(void* argVal);
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
    void decodeKernelDataParameterToken(const PatchDataParameterBuffer* token);
    uint32_t setArgImmediate(void* argValue);
    uint32_t setArgBuffer(void* argValue);
    uint32_t setArgument(uint32_t argIndex, void* argValue);
    int disassembleBinary();
    void setOptBit(uint32_t& opts, uint32_t bit, bool isSet);
    int extractMetadata();
    int createSipKernel();

    Context* context;
  private:
    std::unique_ptr<DeviceDescriptor> descriptor;
    const char* igcName;
    const char* fclName;
    CIFMain* igcMain = nullptr;
    CIFMain* fclMain = nullptr;

    const char* filename;
    DataStruct options;    
    DataStruct sourceCode;
    DataStruct intermediateRepresentation;
    DataStruct deviceBinary;

    uint64_t srcType;
    uint64_t intermediateType;
    uint64_t outType;

    const uint8_t* header = nullptr;
    const uint8_t* patchListBlob = nullptr;
    const uint8_t* kernelInfoBlob = nullptr;
    KernelFromPatchtokens kernelData;
    std::vector<KernelArgHandler> argHandlers;
    std::vector<ArgDescPointer> kernelArgs;
};




