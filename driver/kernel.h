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
class Kernel;
struct DeviceDescriptor;
struct BufferObject;

enum PATCH_TOKEN {
    PATCH_TOKEN_SAMPLER_STATE_ARRAY = 5,
    PATCH_TOKEN_BINDING_TABLE_STATE = 8,
    PATCH_TOKEN_GLOBAL_MEMORY_OBJECT_KERNEL_ARGUMENT = 11,
    PATCH_TOKEN_IMAGE_MEMORY_OBJECT_KERNEL_ARGUMENT = 12,
    PATCH_TOKEN_SAMPLER_KERNEL_ARGUMENT = 16,
    PATCH_TOKEN_DATA_PARAMETER_BUFFER = 17,
    PATCH_TOKEN_MEDIA_VFE_STATE = 18,
    PATCH_TOKEN_MEDIA_INTERFACE_DESCRIPTOR_LOAD = 19,
    PATCH_TOKEN_INTERFACE_DESCRIPTOR_DATA = 21,
    PATCH_TOKEN_THREAD_PAYLOAD = 22,
    PATCH_TOKEN_EXECUTION_ENVIRONMENT = 23,
    PATCH_TOKEN_DATA_PARAMETER_STREAM = 25,
    PATCH_TOKEN_KERNEL_ATTRIBUTES_INFO = 27,
    PATCH_TOKEN_STATELESS_GLOBAL_MEMORY_OBJECT_KERNEL_ARGUMENT = 30,
    PATCH_TOKEN_STATELESS_CONSTANT_MEMORY_OBJECT_KERNEL_ARGUMENT = 31,
    PATCH_TOKEN_STATELESS_DEVICE_QUEUE_KERNEL_ARGUMENT = 46,
    PATCH_TOKEN_MEDIA_VFE_STATE_SLOT1 = 55
};

enum DATA_PARAMETER {
    DATA_PARAMETER_KERNEL_ARGUMENT = 1,
    DATA_PARAMETER_LOCAL_WORK_SIZE = 2,
    DATA_PARAMETER_GLOBAL_WORK_SIZE = 3,
    DATA_PARAMETER_NUM_WORK_GROUPS = 4,
    DATA_PARAMETER_WORK_DIMENSIONS = 5,
    DATA_PARAMETER_IMAGE_WIDTH = 9,
    DATA_PARAMETER_IMAGE_HEIGHT = 10,
    DATA_PARAMETER_IMAGE_DEPTH = 11,
    DATA_PARAMETER_IMAGE_CHANNEL_DATA_TYPE = 12,
    DATA_PARAMETER_IMAGE_CHANNEL_ORDER = 13,
    DATA_PARAMETER_SAMPLER_ADDRESS_MODE = 14,
    DATA_PARAMETER_SAMPLER_NORMALIZED_COORDS = 15,
    DATA_PARAMETER_GLOBAL_WORK_OFFSET = 16,
    DATA_PARAMETER_IMAGE_ARRAY_SIZE = 18,
    DATA_PARAMETER_IMAGE_NUM_SAMPLES = 20,
    DATA_PARAMETER_SAMPLER_COORDINATE_SNAP_WA_REQUIRED = 21,
    DATA_PARAMETER_VME_MB_BLOCK_TYPE = 23,
    DATA_PARAMETER_VME_SUBPIXEL_MODE = 24,
    DATA_PARAMETER_VME_SAD_ADJUST_MODE = 25,
    DATA_PARAMETER_VME_SEARCH_PATH_TYPE = 26,
    DATA_PARAMETER_IMAGE_NUM_MIP_LEVELS = 27,
    DATA_PARAMETER_ENQUEUED_LOCAL_WORK_SIZE = 28,
    DATA_PARAMETER_OBJECT_ID = 35,
    DATA_PARAMETER_BUFFER_STATEFUL = 43,
    DATA_PARAMETER_FLAT_IMAGE_BASEOFFSET = 44,
    DATA_PARAMETER_FLAT_IMAGE_WIDTH = 45,
    DATA_PARAMETER_FLAT_IMAGE_HEIGHT = 46,
    DATA_PARAMETER_FLAT_IMAGE_PITCH = 47
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

struct PatchThreadPayload : PatchItemHeader {
    uint32_t HeaderPresent;
    uint32_t LocalIDXPresent;
    uint32_t LocalIDYPresent;
    uint32_t LocalIDZPresent;
    uint32_t LocalIDFlattenedPresent;
    uint32_t IndirectPayloadStorage;
    uint32_t UnusedPerThreadConstantPresent;
    uint32_t GetLocalIDPresent;
    uint32_t GetGroupIDPresent;
    uint32_t GetGlobalOffsetPresent;
    uint32_t StageInGridOriginPresent;
    uint32_t StageInGridSizePresent;
    uint32_t OffsetToSkipPerThreadDataLoad;
    uint32_t OffsetToSkipSetFFIDGP;
    uint32_t PassInlineData;
};

struct PatchDataParameterStream : PatchItemHeader {
    uint32_t DataParameterStreamSize;
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

struct PatchGlobalMemoryObjectKernelArgument : PatchItemHeader {
    uint32_t ArgumentNumber;
    uint32_t SurfaceStateHeapOffset;
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
    uint32_t ArgumentNumber;
    uint32_t SurfaceStateHeapOffset;
    uint32_t DataParamOffset;
    uint32_t DataParamSize;
    uint32_t LocationIndex;
    uint32_t LocationIndex2;
    uint32_t IsEmulationArgument;
};

#pragma pack ( pop )

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
    const PatchDataParameterStream* dataParameterStream = nullptr;
    const PatchKernelAttributesInfo* kernelAttributesInfo = nullptr;
    const PatchThreadPayload* threadPayload = nullptr;
    struct {
        const PatchDataParameterBuffer* localWorkSize[3] = {nullptr, nullptr, nullptr};
        const PatchDataParameterBuffer* localWorkSize2[3] = {nullptr, nullptr, nullptr};
        const PatchDataParameterBuffer* enqueuedLocalWorkSize[3] = {nullptr, nullptr, nullptr};
        const PatchDataParameterBuffer* numWorkGroups[3] = {nullptr, nullptr, nullptr};
        const PatchDataParameterBuffer* globalWorkOffset[3] = {nullptr, nullptr, nullptr};
        const PatchDataParameterBuffer* globalWorkSize[3] = {nullptr, nullptr, nullptr};
        const PatchDataParameterBuffer* workDimensions = nullptr;
    } crossThreadPayload;
};

struct ArgDescriptor {
    const PatchItemHeader* header = nullptr;
    int argToken = 0;
    int (Kernel::*KernelArgHandler)(uint32_t argNum, size_t argSize, void* argVal);
};

struct ArgDescPointer : ArgDescriptor {
    uint16_t bindful = 0u;
    uint16_t bindless = 0u;
    uint16_t stateless = 0u;
};

struct ArgDescValue : ArgDescriptor {
    uint16_t crossThreadDataOffset = 0u;
    uint16_t size = 0u;
    uint16_t sourceOffset = 0u;
};


namespace codeType {
    constexpr uint64_t oclC = 2305843009183132750;
    constexpr uint64_t spirV = 2305843009202725362;
    constexpr uint64_t oclGenBin = 18425635491780865102;    //TODO: Check the size
};


class Kernel : public pKernel {
  public:
    Kernel(Context* context);
    ~Kernel();
    static int loadCompiler(const char* libName, CIFMain** cifMain);
    char* getSurfaceStatePtr();
    char* getCrossThreadData();
    std::vector<BufferObject*> getExecData();
    IgcBuffer* loadProgramSource(const char* filename);
    int initialize();
    int build(const char* filename, const char* options, uint16_t chipsetID);
    int setArgument(uint32_t argIndex, size_t argSize, void* argValue);
    int disassembleBinary();
    int extractMetadata();
    int retrieveSystemRoutineInstructions();

    Context* context;
  private:
    ICIF* createInterface(CIFMain* cifMain, uint64_t interfaceID, uint64_t interfaceVersion);
    IgcBuffer* createIgcBuffer(CIFMain* cifMain, const char* data, size_t size);
    FclOclDeviceCtx* getFclDeviceCtx();
    IgcOclDeviceCtx* getIgcDeviceCtx();
    void transferPlatformInfo(PlatformInfo* igcPlatform, Platform* platform);
    void transferSystemInfo(GTSystemInfo* igcGetSystemInfo, SystemInfo* gtSystemInfo);
    void transferFeaturesInfo(IgcFeaturesAndWorkarounds* igcFeWa, FeatureTable* featureTable);
    void decodeToken(const PatchItemHeader* token, KernelFromPatchtokens* kernelData);
    void decodeKernelDataParameterToken(const PatchDataParameterBuffer* token);
    void populateKernelArg(uint32_t argNum, uint32_t surfaceStateHeapOffset, uint32_t dataParamOffset);
    int setArgImmediate(uint32_t argIndex, size_t argSize, void* argValue);
    int setArgBuffer(uint32_t argIndex, size_t argSize, void* argValue);
    void clearFclBuffers(FclOclDeviceCtx* deviceCtx, FclOclTranslationCtx* translationCtx, OclTranslationOutput* output);
    void clearIgcBuffers(IgcBuffer* ids, IgcBuffer* values, IgcOclTranslationCtx* translationCtx, OclTranslationOutput* output);
    void clearSystemRoutineBuffers(IgcBuffer* systemRoutine, IgcBuffer* stateSaveAreaHeader);
    void setOptBit(uint32_t& opts, uint32_t bit, bool isSet);

    IgcOclDeviceCtx* igcDeviceCtx = nullptr;
    IgcBuffer* igcBuildOutput = nullptr;
    const char* deviceBinary = nullptr;
    //size_t deviceBinarySize = 0u;

    const uint8_t* header = nullptr;
    const uint8_t* patchListBlob = nullptr;
    const uint8_t* kernelInfoBlob = nullptr;
    KernelFromPatchtokens kernelData;

    bool unsupportedKernelArgs = false;
    bool hasBindlessMode = false;

    std::unique_ptr<DeviceDescriptor> descriptor;
    std::vector<std::unique_ptr<ArgDescriptor>> argDescriptor;
    std::unique_ptr<char[]> sshLocal;
    std::unique_ptr<char[]> crossThreadData;
    std::vector<BufferObject*> execData;
};




