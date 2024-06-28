#include <dlfcn.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <cstring>

#include "buffer.h"
#include "commands_gen9.h"
#include "device.h"
#include "gpgpu.h"
#include "hwinfo.h"
#include "igc_interface.h"
#include "kernel.h"
#include "utils.h"


Kernel::Kernel(Context* context, const char* filename, const char* options) 
         : context(context),
           filename(filename),
           srcType(2305843009183132750),            // oclC
           intermediateType(2305843009202725362),   // spirV
           outType(18425635491780865102) {          // oclGenBin
    this->options.data = options;
    this->options.dataLength = strlen(options);
}

Kernel::~Kernel() {
    printf("Kernel destructor called!\n");
}

char* Kernel::getSurfaceStatePtr() {
    return this->sshLocal.get();
}

char* Kernel::getCrossThreadData() {
    return this->crossThreadData.get();
}

int Kernel::loadProgramSource() {
    FILE* file = fopen(filename, "r");
    if (!file) {
        return LOAD_SOURCE_FAILED;
    }
    fseek(file, 0, SEEK_END);
    uint64_t size = ftell(file);
    rewind(file);

    char* source = new char[size+1];        //TODO: Memory leak (1091 bytes)
    fread(static_cast<void*>(source), 1, size*sizeof(char), file);
    source[size] = '\0';
    fclose(file);

    sourceCode.data = static_cast<const char*>(source);
    sourceCode.dataLength = strlen(source + 1);
    //printf("%s\n", srcCode);
    return SUCCESS;
}

ICIF* Kernel::CreateInterface(CIFMain* cifMain, uint64_t interfaceID, uint64_t interfaceVersion) {
    uint64_t chosenVersion;
    uint64_t minVerSupported = 0;
    uint64_t maxVerSupported = 0;

    bool isSupported = cifMain->GetSupportedVersions(interfaceID, minVerSupported, maxVerSupported);
    //printf("maxVerSupported: %lu\n", maxVerSupported);
    //printf("minVerSupported: %lu\n", minVerSupported);
    if (isSupported == false) {
        return nullptr;
    }
    if ((interfaceVersion < minVerSupported) || (interfaceVersion > maxVerSupported)) {
        return nullptr;
    }
    //printf("Versions are ok\n");
    chosenVersion = std::min(maxVerSupported, interfaceVersion);
    
    ICIF* deviceCtx = cifMain->CreateInterfaceImpl(interfaceID, chosenVersion);     //TODO: Memory leak
    return deviceCtx;
}

IgcBuffer* Kernel::CreateIgcBuffer(CIFMain* cifMain, const char* data, size_t size) {
    if (cifMain == nullptr) {
        return nullptr;
    }
    uint64_t interfaceID = 0xfffe2429681d9502;
    uint64_t interfaceVersion = 1;
    auto buff = CreateInterface(cifMain, interfaceID, interfaceVersion);    //TODO: Memory leak
    IgcBuffer* buffer = static_cast<IgcBuffer*>(buff);
    if (buffer == nullptr) {
        return nullptr;
    }
    if ((data != nullptr) && (size != 0)) {
        buffer->SetUnderlyingStorage(static_cast<const void*>(data), size);
    }
    return buffer;
}

int Kernel::loadCompiler(const char* libName, CIFMain** cifMain) {
    void* handle;
    auto dlopenFlag = RTLD_LAZY | RTLD_DEEPBIND;
    handle = dlopen(libName, dlopenFlag);
    if (!handle) {
        return COMPILER_LOAD_FAILED;
    }
    CIFMain* (*CreateCIFMainFunc)();
    void* addr = dlsym(handle, "CIFCreateMain");
    CreateCIFMainFunc = reinterpret_cast<CIFMain*(*)()>(addr);
    if (CreateCIFMainFunc == nullptr) {
        return COMPILER_LOAD_FAILED;
    }
    *cifMain = CreateCIFMainFunc();     //TODO: Memory leak (24 bytes)
    if (*cifMain == nullptr) {
        return COMPILER_LOAD_FAILED;
    }
    return SUCCESS;
}

FclOclTranslationCtx* Kernel::createFclTranslationCtx() {
    uint64_t interfaceID = 95846467711642693;
    uint64_t interfaceVersion = 5;
    ICIF* DeviceCtx = CreateInterface(context->fclMain, interfaceID, interfaceVersion);     //TODO: Memory leak (152 bytes)
    FclOclDeviceCtx* newDeviceCtx = static_cast<FclOclDeviceCtx*>(DeviceCtx);
    if (newDeviceCtx == nullptr) {
        return nullptr;
    }
    uint32_t openCLVersion = 30;
    newDeviceCtx->SetOclApiVersion(openCLVersion * 10);
    if (newDeviceCtx->GetUnderlyingVersion() > 4U) {
        uint64_t platformVersion = 1;
        PlatformInfo* igcPlatform = newDeviceCtx->GetPlatformHandleImpl(platformVersion);
        if (nullptr == igcPlatform) {
            return nullptr;
        }
        TransferPlatformInfo(igcPlatform, descriptor->pHwInfo->platform);
    }
    uint64_t translationCtxVersion = 1;
    auto fclTranslationCtx = newDeviceCtx->CreateTranslationCtxImpl(translationCtxVersion, srcType, intermediateType);      //TODO: Memory leak (280 bytes)
    return fclTranslationCtx;
}

void Kernel::TransferPlatformInfo(PlatformInfo* igcPlatform, Platform* platform) {
    igcPlatform->SetProductFamily(platform->eProductFamily);
    igcPlatform->SetPCHProductFamily(platform->ePCHProductFamily);
    igcPlatform->SetDisplayCoreFamily(platform->eDisplayCoreFamily);
    igcPlatform->SetRenderCoreFamily(platform->eRenderCoreFamily);
    igcPlatform->SetPlatformType(platform->ePlatformType);
    igcPlatform->SetDeviceID(platform->usDeviceID);
    igcPlatform->SetRevId(platform->usRevId);
    igcPlatform->SetDeviceID_PCH(platform->usDeviceID_PCH);
    igcPlatform->SetRevId_PCH(platform->usRevId_PCH);
    //igcPlatform->SetGTType(platform->eGTType);
    igcPlatform->SetGTType(GTTYPE::GTTYPE_GT3);
    //uint64_t fam = igcPlatform->GetProductFamily();
    //uint64_t core = igcPlatform->GetRenderCoreFamily();
    //printf("fam: %lu, %lu\n", fam, core);
}

void Kernel::TransferSystemInfo(GTSystemInfo* igcGetSystemInfo, SystemInfo* gtSystemInfo) {
    igcGetSystemInfo->SetEuCount(gtSystemInfo->EUCount);
    igcGetSystemInfo->SetThreadCount(gtSystemInfo->ThreadCount);
    igcGetSystemInfo->SetSliceCount(gtSystemInfo->SliceCount);
    igcGetSystemInfo->SetSubSliceCount(gtSystemInfo->SubSliceCount);
    igcGetSystemInfo->SetL3CacheSizeInKb(gtSystemInfo->L3CacheSizeInKb);
    igcGetSystemInfo->SetLLCCacheSizeInKb(gtSystemInfo->LLCCacheSizeInKb);
    //igcGetSystemInfo->SetEdramSizeInKb(gtSystemInfo->EdramSizeInKb);
    igcGetSystemInfo->SetEdramSizeInKb(65536);
    igcGetSystemInfo->SetL3BankCount(gtSystemInfo->L3BankCount);
    igcGetSystemInfo->SetMaxFillRate(gtSystemInfo->MaxFillRate);
    igcGetSystemInfo->SetEuCountPerPoolMax(gtSystemInfo->EuCountPerPoolMax);
    igcGetSystemInfo->SetEuCountPerPoolMin(gtSystemInfo->EuCountPerPoolMin);
    igcGetSystemInfo->SetTotalVsThreads(gtSystemInfo->TotalVsThreads);
    igcGetSystemInfo->SetTotalHsThreads(gtSystemInfo->TotalHsThreads);
    igcGetSystemInfo->SetTotalDsThreads(gtSystemInfo->TotalDsThreads);
    igcGetSystemInfo->SetTotalGsThreads(gtSystemInfo->TotalGsThreads);
    igcGetSystemInfo->SetTotalPsThreadsWindowerRange(gtSystemInfo->TotalPsThreadsWindowerRange);
    igcGetSystemInfo->SetCsrSizeInMb(gtSystemInfo->CsrSizeInMb);
    igcGetSystemInfo->SetMaxEuPerSubSlice(gtSystemInfo->MaxEuPerSubSlice);
    igcGetSystemInfo->SetMaxSlicesSupported(gtSystemInfo->MaxSlicesSupported);
    igcGetSystemInfo->SetMaxSubSlicesSupported(gtSystemInfo->MaxSubSlicesSupported);
    igcGetSystemInfo->SetIsL3HashModeEnabled(gtSystemInfo->IsL3HashModeEnabled);
    igcGetSystemInfo->SetIsDynamicallyPopulated(gtSystemInfo->IsDynamicallyPopulated);
}

void Kernel::TransferFeaturesInfo(IgcFeaturesAndWorkarounds* igcFeWa, FeatureTable* featureTable) {
    igcFeWa->SetFtrDesktop(featureTable->flags.ftrDesktop);
    igcFeWa->SetFtrChannelSwizzlingXOREnabled(featureTable->flags.ftrChannelSwizzlingXOREnabled);
    igcFeWa->SetFtrGtBigDie(featureTable->flags.ftrGtBigDie);
    igcFeWa->SetFtrGtMediumDie(featureTable->flags.ftrGtMediumDie);
    igcFeWa->SetFtrGtSmallDie(featureTable->flags.ftrGtSmallDie);
    igcFeWa->SetFtrGT1(featureTable->flags.ftrGT1);
    igcFeWa->SetFtrGT1_5(featureTable->flags.ftrGT1_5);
    igcFeWa->SetFtrGT2(featureTable->flags.ftrGT2);
    igcFeWa->SetFtrGT3(featureTable->flags.ftrGT3);
    igcFeWa->SetFtrGT4(featureTable->flags.ftrGT4);
    igcFeWa->SetFtrIVBM0M1Platform(featureTable->flags.ftrIVBM0M1Platform);
    igcFeWa->SetFtrGTL(featureTable->flags.ftrGT1);
    igcFeWa->SetFtrGTM(featureTable->flags.ftrGT2);
    igcFeWa->SetFtrGTH(featureTable->flags.ftrGT3);
    igcFeWa->SetFtrSGTPVSKUStrapPresent(featureTable->flags.ftrSGTPVSKUStrapPresent);
    igcFeWa->SetFtrGTA(featureTable->flags.ftrGTA);
    igcFeWa->SetFtrGTC(featureTable->flags.ftrGTC);
    igcFeWa->SetFtrGTX(featureTable->flags.ftrGTX);
    igcFeWa->SetFtr5Slice(featureTable->flags.ftr5Slice);
    igcFeWa->SetFtrGpGpuMidThreadLevelPreempt(featureTable->flags.ftrGpGpuMidThreadLevelPreempt);
    igcFeWa->SetFtrWddm2Svm(featureTable->flags.ftrWddm2Svm);
    igcFeWa->SetFtrPooledEuEnabled(featureTable->flags.ftrPooledEuEnabled);
}

IgcOclDeviceCtx* Kernel::getIgcDeviceCtx() {
    uint64_t interfaceID = 0x15483dac4ed88c8;
    uint64_t interfaceVersion = 2;
    ICIF* DeviceCtx = CreateInterface(context->igcMain, interfaceID, interfaceVersion);     //TODO: Memory leak (32 bytes)
    IgcOclDeviceCtx* newDeviceCtx = static_cast<IgcOclDeviceCtx*>(DeviceCtx);
    if (newDeviceCtx == nullptr) {
        return nullptr;
    }
    int outProfilingTimerResolution = 83;
    newDeviceCtx->SetProfilingTimerResolution(static_cast<float>(outProfilingTimerResolution));
    uint64_t platformID = 1;
    uint64_t gtsystemID = 3;
    uint64_t featureID = 2;
    auto igcPlatform = newDeviceCtx->GetPlatformHandleImpl(platformID);
    auto igcGetSystemInfo = newDeviceCtx->GetGTSystemInfoHandleImpl(gtsystemID);
    auto igcFeWa = newDeviceCtx->GetIgcFeaturesAndWorkaroundsHandleImpl(featureID);
    if (!igcPlatform || !igcGetSystemInfo || !igcFeWa) {
        return nullptr;
    }
    TransferPlatformInfo(igcPlatform, descriptor->pHwInfo->platform);
    TransferSystemInfo(igcGetSystemInfo, descriptor->pHwInfo->gtSystemInfo);
    TransferFeaturesInfo(igcFeWa, descriptor->pHwInfo->featureTable);

    return newDeviceCtx;
}


int Kernel::build(uint16_t chipset_id) {
    int ret = loadProgramSource();
    if (ret)
        return ret;
    if (chipset_id) {
        descriptor = context->device->getDevInfoFromDescriptorTable(chipset_id);
    }
    else {
        descriptor = std::make_unique<DeviceDescriptor>();
        DeviceDescriptor* dd = context->device->getDeviceDescriptor();
        descriptor->pHwInfo = dd->pHwInfo;
        descriptor->setupHardwareInfo = dd->setupHardwareInfo;
        descriptor->devName = dd->devName;
    }
    IgcBuffer* src = CreateIgcBuffer(context->igcMain, sourceCode.data, sourceCode.dataLength + 1);     //TODO: Memory leak (1202 bytes)
    IgcBuffer* buildOptions = CreateIgcBuffer(context->igcMain, options.data, options.dataLength + 1);  //TODO: Memory leak (112 bytes)
    printf("options: %s\n", options.data);

    //TODO: Look into cl_device_caps.cpp to retrieve these from hardware info
    const char* internal_options = "-ocl-version=300 -cl-disable-zebin -cl-intel-has-buffer-offset-arg -D__IMAGE_SUPPORT__=1 -fpreserve-vec3-type -cl-ext=-all,+cl_khr_byte_addressable_store,+cl_khr_fp16,+cl_khr_global_int32_base_atomics,+cl_khr_global_int32_extended_atomics,+cl_khr_icd,+cl_khr_local_int32_base_atomics,+cl_khr_local_int32_extended_atomics,+cl_intel_command_queue_families,+cl_intel_subgroups,+cl_intel_required_subgroup_size,+cl_intel_subgroups_short,+cl_khr_spir,+cl_intel_accelerator,+cl_intel_driver_diagnostics,+cl_khr_priority_hints,+cl_khr_throttle_hints,+cl_khr_create_command_queue,+cl_intel_subgroups_char,+cl_intel_subgroups_long,+cl_khr_il_program,+cl_intel_mem_force_host_memory,+cl_khr_subgroup_extended_types,+cl_khr_subgroup_non_uniform_vote,+cl_khr_subgroup_ballot,+cl_khr_subgroup_non_uniform_arithmetic,+cl_khr_subgroup_shuffle,+cl_khr_subgroup_shuffle_relative,+cl_khr_subgroup_clustered_reduce,+cl_intel_device_attribute_query,+cl_khr_suggested_local_work_size,+cl_khr_fp64,+cl_khr_subgroups,+cl_intel_spirv_device_side_avc_motion_estimation,+cl_intel_spirv_media_block_io,+cl_intel_spirv_subgroups,+cl_khr_spirv_no_integer_wrap_decoration,+cl_intel_unified_shared_memory_preview,+cl_khr_mipmap_image,+cl_khr_mipmap_image_writes,+cl_intel_planar_yuv,+cl_intel_packed_yuv,+cl_intel_motion_estimation,+cl_intel_device_side_avc_motion_estimation,+cl_intel_advanced_motion_estimation,+cl_khr_int64_base_atomics,+cl_khr_int64_extended_atomics,+cl_khr_image2d_from_buffer,+cl_khr_depth_images,+cl_khr_3d_image_writes,+cl_intel_media_block_io,+cl_intel_va_api_media_sharing,+cl_intel_sharing_format_query,+cl_khr_pci_bus_info";
    size_t internalOptionsSize = strlen(internal_options) + 1;
    IgcBuffer* internalOptions = CreateIgcBuffer(context->igcMain, internal_options, internalOptionsSize); //TODO: Memory leak (112 bytes)
    printf("internalOptionsSize: %lu\n", internalOptionsSize);
    IgcBuffer* idsBuffer = CreateIgcBuffer(context->igcMain, nullptr, 0);       //TODO: Memory leak (112 bytes)
    IgcBuffer* valuesBuffer = CreateIgcBuffer(context->igcMain, nullptr, 0);    //TODO: Memory leak (112 bytes)

    // Frontend Compilation
    FclOclTranslationCtx* fclTranslationCtx = createFclTranslationCtx();
    if (!fclTranslationCtx) {
        return FRONTEND_BUILD_ERROR;
    }
    auto fclOutput = fclTranslationCtx->TranslateImpl(1, src, buildOptions, internalOptions, nullptr, 0);       //TODO: Memory leak (5520 bytes)
    if (!fclOutput) {
        return FRONTEND_BUILD_ERROR;
    }
    if (fclOutput->Successful() == false) {
        return FRONTEND_BUILD_ERROR;
    }
    IgcBuffer* fclBuildLog = fclOutput->GetBuildLogImpl(1);
    const char* fclBuildLogMem = reinterpret_cast<const char*>(fclBuildLog->GetMemoryRaw());
    if (fclBuildLogMem) {
        printf("%s\n", fclBuildLogMem);
    }
    IgcBuffer* fclBuildOutput = fclOutput->GetOutputImpl(1);
    intermediateRepresentation.data = reinterpret_cast<const char*>(fclBuildOutput->GetMemoryRaw());
    intermediateRepresentation.dataLength = fclBuildOutput->GetSizeRaw();
    if (!intermediateRepresentation.data) {
        return FRONTEND_BUILD_ERROR; 
    }
    printf("%s\n", intermediateRepresentation.data);
    fclBuildOutput->Retain();

    // Backend Compilation
    deviceCtx = getIgcDeviceCtx();
    if (!deviceCtx) {
        return BACKEND_BUILD_ERROR;
    }
    uint64_t translationCtxVersion = 3;
    IgcOclTranslationCtx* igcTranslationCtx = deviceCtx->CreateTranslationCtxImpl(translationCtxVersion, intermediateType, outType);        //TODO: Memory leak (2620 bytes)
    if (!igcTranslationCtx) {
        return BACKEND_BUILD_ERROR;
    }
    auto igcOutput = igcTranslationCtx->TranslateImpl(1, fclBuildOutput, idsBuffer, valuesBuffer, buildOptions, internalOptions, nullptr, 0, nullptr);  //TODO: Memory leak (16224 bytes)
    if (!igcOutput) {
        return BACKEND_BUILD_ERROR;
    }
    if (igcOutput->Successful() == false) {
        return BACKEND_BUILD_ERROR;
    }
    IgcBuffer* igcBuildLog = igcOutput->GetBuildLogImpl(1);
    const char* igcBuildLogMem = reinterpret_cast<const char*>(igcBuildLog->GetMemoryRaw());
    if (igcBuildLogMem) {
        printf("%s\n", igcBuildLogMem);
    }
    IgcBuffer* igcBuildOutput = igcOutput->GetOutputImpl(1);
    deviceBinary.data = reinterpret_cast<const char*>(igcBuildOutput->GetMemoryRaw());
    deviceBinary.dataLength = igcBuildOutput->GetSizeRaw();
    if (!deviceBinary.data) {
        return BACKEND_BUILD_ERROR;
    }
    printf("%s\n", deviceBinary.data);
    return SUCCESS;
}


void Kernel::decodeToken(const PatchItemHeader* token, KernelFromPatchtokens* kernelData) {
    switch (token->Token) {
    case PATCH_TOKEN_SAMPLER_STATE_ARRAY:
        kernelData->samplerStateArray = reinterpret_cast<const PatchSamplerStateArray*>(token);
        break;
    case PATCH_TOKEN_BINDING_TABLE_STATE:
        kernelData->bindingTableState = reinterpret_cast<const PatchBindingTableState*>(token);
        break;
    case PATCH_TOKEN_MEDIA_VFE_STATE:
        kernelData->mediaVfeState[0] = reinterpret_cast<const PatchMediaVFEState*>(token);
        break;
    case PATCH_TOKEN_MEDIA_VFE_STATE_SLOT1:
        kernelData->mediaVfeState[1] = reinterpret_cast<const PatchMediaVFEState*>(token);
        break;
    case PATCH_TOKEN_MEDIA_INTERFACE_DESCRIPTOR_LOAD:
        kernelData->mediaInterfaceDescriptorLoad = reinterpret_cast<const PatchMediaInterfaceDescriptorLoad*>(token);
        break;
    case PATCH_TOKEN_INTERFACE_DESCRIPTOR_DATA:
        kernelData->interfaceDescriptorData = reinterpret_cast<const PatchInterfaceDescriptorData*>(token);
        break;
    case PATCH_TOKEN_EXECUTION_ENVIRONMENT:
        kernelData->executionEnvironment = reinterpret_cast<const PatchExecutionEnvironment*>(token);
        break;
    case PATCH_TOKEN_KERNEL_ATTRIBUTES_INFO:
        kernelData->kernelAttributesInfo = reinterpret_cast<const PatchKernelAttributesInfo*>(token);
        break;
    case PATCH_TOKEN_DATA_PARAMETER_STREAM:
        kernelData->dataParameterStream = reinterpret_cast<const PatchDataParameterStream*>(token);
        break;
    case PATCH_TOKEN_DATA_PARAMETER_BUFFER: {
        auto tokenParam = reinterpret_cast<const PatchDataParameterBuffer*>(token);
        decodeKernelDataParameterToken(tokenParam);
        } break;
    case PATCH_TOKEN_GLOBAL_MEMORY_OBJECT_KERNEL_ARGUMENT: {
        auto global = reinterpret_cast<const PatchGlobalMemoryObjectKernelArgument*>(token);
        argDescriptor[global->ArgumentNumber]->header = token;
        argDescriptor[global->ArgumentNumber]->argToken = PATCH_TOKEN_GLOBAL_MEMORY_OBJECT_KERNEL_ARGUMENT;
        } break;
    case PATCH_TOKEN_STATELESS_GLOBAL_MEMORY_OBJECT_KERNEL_ARGUMENT: {
        auto stateless_global = reinterpret_cast<const PatchStatelessGlobalMemoryObjectKernelArgument*>(token);
        argDescriptor[stateless_global->ArgumentNumber]->header = token;
        argDescriptor[stateless_global->ArgumentNumber]->argToken = PATCH_TOKEN_STATELESS_GLOBAL_MEMORY_OBJECT_KERNEL_ARGUMENT;
        } break;
    case PATCH_TOKEN_STATELESS_CONSTANT_MEMORY_OBJECT_KERNEL_ARGUMENT: {
        auto stateless_const = reinterpret_cast<const PatchStatelessConstantMemoryObjectKernelArgument*>(token);
        argDescriptor[stateless_const->ArgumentNumber]->header = token;
        argDescriptor[stateless_const->ArgumentNumber]->argToken = PATCH_TOKEN_STATELESS_CONSTANT_MEMORY_OBJECT_KERNEL_ARGUMENT;
        } break;
    case PATCH_TOKEN_SAMPLER_KERNEL_ARGUMENT:
    case PATCH_TOKEN_IMAGE_MEMORY_OBJECT_KERNEL_ARGUMENT:
    case PATCH_TOKEN_STATELESS_DEVICE_QUEUE_KERNEL_ARGUMENT:
        this->unsupportedKernelArgs = true;
        break;
    default:
        break;
    }
}

void Kernel::decodeKernelDataParameterToken(const PatchDataParameterBuffer* token) {
    uint32_t argNum = token->ArgumentNumber;
    switch (token->Type) {
    case DATA_PARAMETER_KERNEL_ARGUMENT: {
        if (argDescriptor.size() < argNum + 1) {
            argDescriptor.resize(argNum + 1);
        }
        auto valueDesc = std::make_unique<ArgDescValue>();
        valueDesc->header = reinterpret_cast<const PatchItemHeader*>(token);
        valueDesc->argToken = DATA_PARAMETER_BUFFER_STATEFUL;
        valueDesc->KernelArgHandler = &Kernel::setArgImmediate;
        valueDesc->size = token->DataSize;
        valueDesc->crossThreadDataOffset = token->Offset;
        valueDesc->sourceOffset = token->SourceOffset;
        argDescriptor[argNum] = std::move(valueDesc);
        } break;
    case DATA_PARAMETER_BUFFER_STATEFUL: {
        if (argDescriptor.size() < argNum + 1) {
            argDescriptor.resize(argNum + 1);
        }
        auto pointerDesc = std::make_unique<ArgDescPointer>();
        pointerDesc->KernelArgHandler = &Kernel::setArgBuffer;
        argDescriptor[argNum] = std::move(pointerDesc);
        } break;
    case DATA_PARAMETER_LOCAL_WORK_SIZE: {
        uint32_t index = token->SourceOffset >> 2;
        if (index <= 2)
            kernelData.crossThreadPayload.localWorkSize[index] = token;
        } break;
    case DATA_PARAMETER_GLOBAL_WORK_OFFSET: {
        uint32_t index = token->SourceOffset >> 2;
        if (index <= 2)
            kernelData.crossThreadPayload.globalWorkOffset[index] = token;
        } break;
    case DATA_PARAMETER_ENQUEUED_LOCAL_WORK_SIZE: {
        uint32_t index = token->SourceOffset >> 2;
        if (index <= 2)
            kernelData.crossThreadPayload.enqueuedLocalWorkSize[index] = token;
        } break;
    case DATA_PARAMETER_GLOBAL_WORK_SIZE: {
        uint32_t index = token->SourceOffset >> 2;
        if (index <= 2)
            kernelData.crossThreadPayload.globalWorkSize[index] = token;
        } break;
    case DATA_PARAMETER_NUM_WORK_GROUPS: {
        uint32_t index = token->SourceOffset >> 2;
        if (index <= 2)
            kernelData.crossThreadPayload.numWorkGroups[index] = token;
        } break;
    case DATA_PARAMETER_WORK_DIMENSIONS:
        kernelData.crossThreadPayload.workDimensions = token;
        break;
    case DATA_PARAMETER_OBJECT_ID:
    case DATA_PARAMETER_IMAGE_WIDTH:
    case DATA_PARAMETER_IMAGE_HEIGHT:
    case DATA_PARAMETER_IMAGE_DEPTH:
    case DATA_PARAMETER_IMAGE_CHANNEL_DATA_TYPE:
    case DATA_PARAMETER_IMAGE_CHANNEL_ORDER:
    case DATA_PARAMETER_IMAGE_ARRAY_SIZE:
    case DATA_PARAMETER_IMAGE_NUM_SAMPLES:
    case DATA_PARAMETER_IMAGE_NUM_MIP_LEVELS:
    case DATA_PARAMETER_FLAT_IMAGE_BASEOFFSET:
    case DATA_PARAMETER_FLAT_IMAGE_WIDTH:
    case DATA_PARAMETER_FLAT_IMAGE_HEIGHT:
    case DATA_PARAMETER_FLAT_IMAGE_PITCH:
    case DATA_PARAMETER_SAMPLER_COORDINATE_SNAP_WA_REQUIRED:
    case DATA_PARAMETER_SAMPLER_ADDRESS_MODE:
    case DATA_PARAMETER_SAMPLER_NORMALIZED_COORDS:
    case DATA_PARAMETER_VME_MB_BLOCK_TYPE:
    case DATA_PARAMETER_VME_SUBPIXEL_MODE:
    case DATA_PARAMETER_VME_SAD_ADJUST_MODE:
    case DATA_PARAMETER_VME_SEARCH_PATH_TYPE:
        this->unsupportedKernelArgs = true;
        break;
    default:
        break;
    }
}


void Kernel::populateKernelArg(uint32_t argNum, uint32_t surfaceStateHeapOffset, uint32_t dataParamOffset) {
    auto descriptor = static_cast<ArgDescPointer*>(argDescriptor[argNum].get());
    if (kernelData.executionEnvironment->UseBindlessMode) {
        descriptor->bindless = static_cast<uint16_t>(surfaceStateHeapOffset);
        this->hasBindlessMode = true;
    } else {
        descriptor->bindful = static_cast<uint16_t>(surfaceStateHeapOffset);
    }
    if (dataParamOffset) {
        descriptor->stateless = static_cast<uint16_t>(dataParamOffset);
    }
}


int Kernel::setArgument(uint32_t argIndex, size_t argSize, void* argValue) {
    if (argIndex >= argDescriptor.size()) {
        return INVALID_KERNEL_ARG;
    }
    ArgDescriptor* desc = argDescriptor[argIndex].get();
    int ret = (this->*desc->KernelArgHandler)(argIndex, argSize, argValue);
    return ret;
}

int Kernel::setArgImmediate(uint32_t argIndex, size_t argSize, void* argValue) {
    auto argDescValue = static_cast<ArgDescValue*>(argDescriptor[argIndex].get());
    auto pDest = ptrOffset(crossThreadData.get(), argDescValue->crossThreadDataOffset);
    auto pSrc = ptrOffset(argValue, argDescValue->sourceOffset);
    if (argDescValue->sourceOffset < argSize) {
        size_t maxBytesToCopy = argSize - argDescValue->sourceOffset;
        size_t bytesToCopy = std::min(static_cast<size_t>(argDescValue->size), maxBytesToCopy);
        memcpy(pDest, pSrc, bytesToCopy);
    }
    return SUCCESS;
}

//TODO: Add cases for images, command queues, pipes, ...
int Kernel::setArgBuffer(uint32_t argIndex, size_t argSize, void* argValue) {
    if (!argValue)
        return INVALID_KERNEL_ARG;
    Buffer* buffer = static_cast<Buffer*>(argValue);
    if (buffer->magic != 0x373E5A13)
        return INVALID_KERNEL_ARG;
    BufferObject* bufferObject = buffer->getDataBuffer();
    auto descriptor = static_cast<ArgDescPointer*>(argDescriptor[argIndex].get());
    switch (descriptor->argToken) {
    case PATCH_TOKEN_GLOBAL_MEMORY_OBJECT_KERNEL_ARGUMENT: {
        auto global = reinterpret_cast<const PatchGlobalMemoryObjectKernelArgument*>(descriptor->header);
        populateKernelArg(argIndex, global->SurfaceStateHeapOffset, 0);
        } break;
    case PATCH_TOKEN_STATELESS_GLOBAL_MEMORY_OBJECT_KERNEL_ARGUMENT: {
        auto stateless_global = reinterpret_cast<const PatchStatelessGlobalMemoryObjectKernelArgument*>(descriptor->header);
        populateKernelArg(argIndex, stateless_global->SurfaceStateHeapOffset, stateless_global->DataParamOffset);
        } break;
    case PATCH_TOKEN_STATELESS_CONSTANT_MEMORY_OBJECT_KERNEL_ARGUMENT: {
        auto stateless_const = reinterpret_cast<const PatchStatelessConstantMemoryObjectKernelArgument*>(descriptor->header);
        populateKernelArg(argIndex, stateless_const->SurfaceStateHeapOffset, stateless_const->DataParamOffset);
        } break;
    default:
        break;
    }
    //TODO: Check if GROMACS has bindless mode somewhere
    if (descriptor->stateless) {
        uint64_t* patchLocation = reinterpret_cast<uint64_t*>(ptrOffset(crossThreadData.get(), descriptor->stateless));
        *patchLocation = bufferObject->gpuAddress;
    }
    auto surfaceState = reinterpret_cast<RENDER_SURFACE_STATE*>(ptrOffset(sshLocal.get(), descriptor->bindful));
    *surfaceState = RENDER_SURFACE_STATE::init();
    SURFACE_STATE_BUFFER_LENGTH Length = {0};
    size_t dataBufferSize = alignUp(bufferObject->size, 4);
    Length.Length = static_cast<uint32_t>(dataBufferSize - 1);
    surfaceState->Bitfield.Width = Length.SurfaceState.Width;
    surfaceState->Bitfield.Height = Length.SurfaceState.Height;
    surfaceState->Bitfield.Depth = Length.SurfaceState.Depth;
    surfaceState->Bitfield.VerticalLineStride = 0u;
    surfaceState->Bitfield.VerticalLineStrideOffset = 0u;
    uint32_t mocsIndex = getMocsIndex();
    surfaceState->Bitfield.MemoryObjectControlState_Reserved = mocsIndex; // leads to data loss, I don't know why this is necessary
    surfaceState->Bitfield.MemoryObjectControlState_IndexToMocsTables = (mocsIndex >> 1);
    surfaceState->Bitfield.SurfaceBaseAddress = bufferObject->gpuAddress;

    execData.push_back(bufferObject);

    return SUCCESS;
}

uint32_t Kernel::getMocsIndex() {
    //TODO: Add switch-case for index determination
    uint32_t index = 4u;
    return index;
}

int Kernel::extractMetadata() {
    // The following usage of reinterpret_cast could lead to undefined behaviour. Checking the header magic
    // makes sure that the reinterpreted memory has the correct format
    const ProgramBinaryHeader* binHeader = reinterpret_cast<const ProgramBinaryHeader*>(deviceBinary.data);
    if (binHeader->Magic != 0x494E5443) {
        return INVALID_KERNEL_FORMAT;
    }
    header = reinterpret_cast<const uint8_t*>(binHeader);
    patchListBlob = header + sizeof(ProgramBinaryHeader);
    kernelInfoBlob = patchListBlob + binHeader->PatchListSize;
    
    kernelData.header = reinterpret_cast<const KernelBinaryHeader*>(kernelInfoBlob);
    kernelData.kernelInfo = kernelInfoBlob;
    kernelData.name = kernelInfoBlob + sizeof(KernelBinaryHeader);
    kernelData.isa = kernelData.name + kernelData.header->KernelNameSize;
    kernelData.generalState = kernelData.isa + kernelData.header->KernelHeapSize;
    kernelData.dynamicState = kernelData.generalState + kernelData.header->GeneralStateHeapSize;
    kernelData.surfaceState = kernelData.dynamicState + kernelData.header->DynamicStateHeapSize;
    kernelData.patchList = kernelData.surfaceState + kernelData.header->SurfaceStateHeapSize;
    kernelData.patchListEnd = kernelData.patchList + kernelData.header->PatchListSize;
    const uint8_t* decodePos = kernelData.patchList;
    while (static_cast<uint64_t>(kernelData.patchListEnd - decodePos) > sizeof(PatchItemHeader)) {
        const PatchItemHeader* token = reinterpret_cast<const PatchItemHeader*>(decodePos);
        decodeToken(token, &kernelData);
        decodePos = decodePos + token->Size;
    }
    context->setKernelData(&kernelData);
    //TODO: Check if all necessary patchtokens are not nullptr
    if (kernelData.bindingTableState == nullptr)
        return INVALID_KERNEL_FORMAT;
    printf("bindingTableState->Count = %u\n", kernelData.bindingTableState->Count);
    if (unsupportedKernelArgs || hasBindlessMode)
        return INVALID_KERNEL_FORMAT;
    //TODO: Check if GROMACS uses implicit args
    uint32_t crossThreadDataSize = kernelData.dataParameterStream->DataParameterStreamSize;
    if (crossThreadDataSize) {
        crossThreadData = std::make_unique<char[]>(crossThreadDataSize);
        memset(crossThreadData.get(), 0x00, crossThreadDataSize);
    }
    uint32_t sshSize = kernelData.header->SurfaceStateHeapSize;
    if (sshSize) {
        sshLocal = std::make_unique<char[]>(sshSize);
        memcpy(sshLocal.get(), kernelData.surfaceState, sshSize);
    }
    return SUCCESS;
}

int Kernel::retrieveSystemRoutineInstructions() {
    if (!deviceCtx)
        deviceCtx = getIgcDeviceCtx();
    uint64_t interfaceID2 = 0xfffe2429681d9502;
    uint64_t interfaceVersion2 = 0x1;
    ICIF* RoutineBufferCtx = CreateInterface(context->igcMain, interfaceID2, interfaceVersion2);    //TODO: Memory leak (5489 bytes)
    ICIF* AreaBufferCtx = CreateInterface(context->igcMain, interfaceID2, interfaceVersion2);       //TODO: Memory leak (112 bytes)
    IgcBuffer* systemRoutineBuffer = static_cast<IgcBuffer*>(RoutineBufferCtx);
    IgcBuffer* stateSaveAreaBuffer = static_cast<IgcBuffer*>(AreaBufferCtx);
    if (!systemRoutineBuffer || !stateSaveAreaBuffer)
        return SIP_ERROR;
    uint64_t typeOfSystemRoutine = 0xfffffffffffc642;
    bool result = deviceCtx->GetSystemRoutine(typeOfSystemRoutine, false, systemRoutineBuffer, stateSaveAreaBuffer);    
    if (!result)
        return SIP_ERROR;
    size_t sipSize = systemRoutineBuffer->GetSizeRaw();
    const char* sipBinaryRaw = static_cast<const char*>(systemRoutineBuffer->GetMemoryRaw());
    if (!sipSize || !sipBinaryRaw)
        return SIP_ERROR;
    int ret = context->createSipAllocation(sipSize, sipBinaryRaw);
    if (ret)
        return ret;
    return SUCCESS;
}


void Kernel::setOptBit(uint32_t& opts, uint32_t bit, bool isSet) {
    if (isSet) {
        opts |= bit;
    } else {
        opts &= ~bit;
    }
}

int Kernel::disassembleBinary() {
    /*
    iga_disassemble_options_t dopts = {sizeof(iga_disassemble_options_t), IGA_FORMATTING_OPTS_DEFAULT, 0, 0, IGA_DECODING_OPTS_DEFAULT};
    uint32_t fmtOpts = 0;
    setOptBit(fmtOpts, IGA_FORMATTING_OPT_NUMERIC_LABELS, false);
    setOptBit(fmtOpts, IGA_FORMATTING_OPT_SYNTAX_EXTS, false);
    setOptBit(fmtOpts, IGA_FORMATTING_OPT_PRINT_HEX_FLOATS, false);
    setOptBit(fmtOpts, IGA_FORMATTING_OPT_PRINT_PC, false);
    setOptBit(fmtOpts, IGA_FORMATTING_OPT_PRINT_BITS, false);
    setOptBit(fmtOpts, IGA_FORMATTING_OPT_PRINT_DEFS, false);
    setOptBit(fmtOpts, IGA_FORMATTING_OPT_PRINT_DEPS, false);
    setOptBit(fmtOpts, IGA_FORMATTING_OPT_PRINT_LDST, false);
    setOptBit(fmtOpts, IGA_FORMATTING_OPT_PRINT_BFNEXPRS, true);
    setOptBit(fmtOpts, IGA_FORMATTING_OPT_PRINT_ANSI, true);
    setOptBit(fmtOpts, IGA_FORMATTING_OPT_PRINT_JSON, false);
    setOptBit(fmtOpts, IGA_FORMATTING_OPT_PRINT_JSON_V1, false);
    dopts.formatting_opts = fmtopts;
    dopts.base_pc_offset = pcOffset;
    setOptBit(dopts.decoder_opts, IGA_DECODING_OPT_NATIVE, useNativeEncoder);

    char* text;
    int status = iga_disassemble(context, &dopts, kernelData.isa, kernelData.header->KernelHeapSize, nullptr, nullptr, &text);
    */
    return SUCCESS;
}






