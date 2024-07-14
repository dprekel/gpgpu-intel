#include <dlfcn.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <cstring>
#include <string>

#include "buffer.h"
#include "commands_gen9.h"
#include "device.h"
#include "gpgpu.h"
#include "hwinfo.h"
#include "igc_interface.h"
#include "kernel.h"
#include "utils.h"


Kernel::Kernel(Context* context)
         : context(context) {
}

Kernel::~Kernel() {
    DBG_LOG("[DEBUG] Kernel destructor called!\n");
    if (igcDeviceCtx)
        igcDeviceCtx->Release();
    if (igcBuildOutput)
        igcBuildOutput->Release();
}


char* Kernel::getSurfaceStatePtr() {
    return sshLocal.get();
}

char* Kernel::getCrossThreadData() {
    return crossThreadData.get();
}

std::vector<BufferObject*> Kernel::getExecData() {
    return execData;
}

//TODO: How do I close the compiler?
int Kernel::loadCompiler(const char* libName, CIFMain** cifMain) {
    auto dlopenFlag = RTLD_LAZY | RTLD_DEEPBIND;
    std::string directory = ""; //"/home/david/igc/igc2/";
    std::string filename = directory + libName;
    void* handle = dlopen(filename.c_str(), dlopenFlag);
    if (!handle) {
        return COMPILER_LOAD_ERROR;
    }
    CIFMain* (*CreateCIFMainFunc)();
    void* addr = dlsym(handle, "CIFCreateMain");
    CreateCIFMainFunc = reinterpret_cast<CIFMain*(*)()>(addr);
    if (CreateCIFMainFunc == nullptr) {
        return COMPILER_LOAD_ERROR;
    }
    *cifMain = CreateCIFMainFunc();
    if (*cifMain == nullptr) {
        return COMPILER_LOAD_ERROR;
    }
    return SUCCESS;
}

//TODO: Check this function, valgrind says there are invalid reads
IgcBuffer* Kernel::loadProgramSource(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file)
        return nullptr;
    fseek(file, 0, SEEK_END);
    uint64_t size = ftell(file);
    rewind(file);

    std::unique_ptr<char[]> source(new char[size + 1]);
    fread(source.get(), 1, size*sizeof(char), file);
    fclose(file);
    source[size] = '\0';
    DBG_LOG("%s\n", source.get());
    auto programSourceBuffer = createIgcBuffer(context->igcMain, source.get(), strlen(source.get()) + 1);
    if (!programSourceBuffer)
        return nullptr;
    return programSourceBuffer;
}

ICIF* Kernel::createInterface(CIFMain* cifMain, uint64_t interfaceID, uint64_t interfaceVersion) {
    if (!cifMain)
        return nullptr;
    uint64_t minVerSupported = 0u;
    uint64_t maxVerSupported = 0u;
    bool isSupported = cifMain->GetSupportedVersions(interfaceID, minVerSupported, maxVerSupported);
    if (!isSupported)
        return nullptr;
    if ((interfaceVersion < minVerSupported) || (interfaceVersion > maxVerSupported))
        return nullptr;
    uint64_t chosenVersion = std::min(maxVerSupported, interfaceVersion);
    ICIF* deviceCtx = cifMain->CreateInterface(interfaceID, chosenVersion);
    return deviceCtx;
}

IgcBuffer* Kernel::createIgcBuffer(CIFMain* cifMain, const char* data, size_t size) {
    if (!cifMain)
        return nullptr;
    uint64_t interfaceID = 0xfffe2429681d9502;
    uint64_t interfaceVersion = 1;
    auto buff = createInterface(cifMain, interfaceID, interfaceVersion);
    IgcBuffer* buffer = static_cast<IgcBuffer*>(buff);
    if (!buffer)
        return nullptr;
    if ((data != nullptr) && (size != 0)) {
        buffer->SetUnderlyingStorage(static_cast<const void*>(data), size);
    }
    return buffer;
}

//TODO: Test the whole process with different kernels
FclOclDeviceCtx* Kernel::getFclDeviceCtx() {
    uint64_t interfaceID = 95846467711642693;
    uint64_t interfaceVersion = 0x5;
    ICIF* DeviceCtx = createInterface(context->fclMain, interfaceID, interfaceVersion);
    FclOclDeviceCtx* newDeviceCtx = static_cast<FclOclDeviceCtx*>(DeviceCtx);
    if (!newDeviceCtx)
        return nullptr;
    FeatureTable* featureTable = deviceDescriptor->pHwInfo->featureTable;
    uint32_t openCLVersion = featureTable->flags.ftrSupportsOcl30 ? 30 : 21;
    newDeviceCtx->SetOclApiVersion(openCLVersion * 10);
    if (newDeviceCtx->GetUnderlyingVersion() > 4u) {
        uint64_t platformVersion = 0x1;
        PlatformInfo* igcPlatform = newDeviceCtx->GetPlatformHandle(platformVersion);
        if (!igcPlatform)
            return nullptr;
        transferPlatformInfo(igcPlatform, deviceDescriptor->pHwInfo->platform);
    }
    return newDeviceCtx;
}

void Kernel::transferPlatformInfo(PlatformInfo* igcPlatform, Platform* platform) {
    igcPlatform->SetProductFamily(platform->eProductFamily);
    igcPlatform->SetPCHProductFamily(platform->ePCHProductFamily);
    igcPlatform->SetDisplayCoreFamily(platform->eDisplayCoreFamily);
    igcPlatform->SetRenderCoreFamily(platform->eRenderCoreFamily);
    igcPlatform->SetPlatformType(platform->ePlatformType);
    igcPlatform->SetDeviceID(platform->usDeviceID);
    igcPlatform->SetRevId(platform->usRevId);
    igcPlatform->SetDeviceID_PCH(platform->usDeviceID_PCH);
    igcPlatform->SetRevId_PCH(platform->usRevId_PCH);
    igcPlatform->SetGTType(platform->eGTType);
}

void Kernel::transferSystemInfo(GTSystemInfo* igcGetSystemInfo, SystemInfo* gtSystemInfo) {
    igcGetSystemInfo->SetEuCount(gtSystemInfo->EUCount);
    igcGetSystemInfo->SetThreadCount(gtSystemInfo->ThreadCount);
    igcGetSystemInfo->SetSliceCount(gtSystemInfo->SliceCount);
    igcGetSystemInfo->SetSubSliceCount(gtSystemInfo->SubSliceCount);
    igcGetSystemInfo->SetL3CacheSizeInKb(gtSystemInfo->L3CacheSizeInKb);
    igcGetSystemInfo->SetLLCCacheSizeInKb(gtSystemInfo->LLCCacheSizeInKb);
    igcGetSystemInfo->SetEdramSizeInKb(gtSystemInfo->EdramSizeInKb);
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


//TODO: Retrieve outProfilingTimerResolution from device info
IgcOclDeviceCtx* Kernel::getIgcDeviceCtx() {
    uint64_t interfaceID = 0x15483dac4ed88c8u;
    uint64_t interfaceVersion = 2u;
    ICIF* DeviceCtx = createInterface(context->igcMain, interfaceID, interfaceVersion);
    IgcOclDeviceCtx* newDeviceCtx = static_cast<IgcOclDeviceCtx*>(DeviceCtx);
    if (!newDeviceCtx)
        return nullptr;
    int outProfilingTimerResolution = 83;
    newDeviceCtx->SetProfilingTimerResolution(static_cast<float>(outProfilingTimerResolution));
    uint64_t platformID = 1u;
    uint64_t gtsystemID = 3u;
    uint64_t featureID = 2u;
    auto igcPlatform = newDeviceCtx->GetPlatformHandle(platformID);
    auto igcGetSystemInfo = newDeviceCtx->GetGTSystemInfoHandle(gtsystemID);
    auto igcFeWa = newDeviceCtx->GetIgcFeaturesAndWorkaroundsHandle(featureID);
    if (!igcPlatform || !igcGetSystemInfo || !igcFeWa) {
        return nullptr;
    }
    transferPlatformInfo(igcPlatform, deviceDescriptor->pHwInfo->platform);
    transferSystemInfo(igcGetSystemInfo, deviceDescriptor->pHwInfo->gtSystemInfo);

    FeatureTable* featureTable = deviceDescriptor->pHwInfo->featureTable;
    igcFeWa->SetFtrGpGpuMidThreadLevelPreempt(featureTable->flags.ftrGpGpuMidThreadLevelPreempt);
    igcFeWa->SetFtrWddm2Svm(featureTable->flags.ftrWddm2Svm);
    igcFeWa->SetFtrPooledEuEnabled(featureTable->flags.ftrPooledEuEnabled);

    return newDeviceCtx;
}



int Kernel::build(const char* filename, const char* buildOptions) {
    auto programSourceBuffer = loadProgramSource(filename);
    if (!programSourceBuffer)
        return SOURCE_LOAD_ERROR;
    deviceDescriptor = context->device->getDeviceDescriptor();
    if (!deviceDescriptor)
        return UNSUPPORTED_HARDWARE;

    //TODO: Look into cl_device_caps.cpp to retrieve these from hardware info
    const char* internalOptions = "-ocl-version=300 -cl-disable-zebin -cl-intel-has-buffer-offset-arg -D__IMAGE_SUPPORT__=1 -fpreserve-vec3-type -cl-ext=-all,+cl_khr_byte_addressable_store,+cl_khr_fp16,+cl_khr_global_int32_base_atomics,+cl_khr_global_int32_extended_atomics,+cl_khr_icd,+cl_khr_local_int32_base_atomics,+cl_khr_local_int32_extended_atomics,+cl_intel_command_queue_families,+cl_intel_subgroups,+cl_intel_required_subgroup_size,+cl_intel_subgroups_short,+cl_khr_spir,+cl_intel_accelerator,+cl_intel_driver_diagnostics,+cl_khr_priority_hints,+cl_khr_throttle_hints,+cl_khr_create_command_queue,+cl_intel_subgroups_char,+cl_intel_subgroups_long,+cl_khr_il_program,+cl_intel_mem_force_host_memory,+cl_khr_subgroup_extended_types,+cl_khr_subgroup_non_uniform_vote,+cl_khr_subgroup_ballot,+cl_khr_subgroup_non_uniform_arithmetic,+cl_khr_subgroup_shuffle,+cl_khr_subgroup_shuffle_relative,+cl_khr_subgroup_clustered_reduce,+cl_intel_device_attribute_query,+cl_khr_suggested_local_work_size,+cl_khr_fp64,+cl_khr_subgroups,+cl_intel_spirv_device_side_avc_motion_estimation,+cl_intel_spirv_media_block_io,+cl_intel_spirv_subgroups,+cl_khr_spirv_no_integer_wrap_decoration,+cl_intel_unified_shared_memory_preview,+cl_khr_mipmap_image,+cl_khr_mipmap_image_writes,+cl_intel_planar_yuv,+cl_intel_packed_yuv,+cl_intel_motion_estimation,+cl_intel_device_side_avc_motion_estimation,+cl_intel_advanced_motion_estimation,+cl_khr_int64_base_atomics,+cl_khr_int64_extended_atomics,+cl_khr_image2d_from_buffer,+cl_khr_depth_images,+cl_khr_3d_image_writes,+cl_intel_media_block_io,+cl_intel_va_api_media_sharing,+cl_intel_sharing_format_query,+cl_khr_pci_bus_info";
    auto internalOptionsBuffer = createIgcBuffer(context->igcMain, internalOptions, strlen(internalOptions) + 1);
    auto buildOptionsBuffer = createIgcBuffer(context->igcMain, buildOptions, strlen(buildOptions) + 1);

    // Frontend Compilation
    auto fclDeviceCtx = getFclDeviceCtx();
    if (!fclDeviceCtx) {
        return FRONTEND_BUILD_ERROR;
    }
    uint64_t fclTranslationCtxVersion = 0x1;
    auto fclTranslationCtx = fclDeviceCtx->CreateTranslationCtx(fclTranslationCtxVersion, codeType::oclC, codeType::spirV);
    if (!fclTranslationCtx) {
        clearFclBuffers(fclDeviceCtx, nullptr, nullptr);
        return FRONTEND_BUILD_ERROR;
    }
    auto fclResult = fclTranslationCtx->Translate(0x1, programSourceBuffer, buildOptionsBuffer, internalOptionsBuffer, nullptr, 0);
    if (!fclResult) {
        clearFclBuffers(fclDeviceCtx, fclTranslationCtx, nullptr);
        return FRONTEND_BUILD_ERROR;
    }
    if (fclResult->Successful() == false) {
        IgcBuffer* fclBuildLog = fclResult->GetBuildLog(0x1);
        const char* fclBuildLogMem = reinterpret_cast<const char*>(fclBuildLog->GetMemoryRaw());
        if (fclBuildLogMem)
            DBG_LOG("[DEBUG] FCL build log:\n%s\n", fclBuildLogMem);
        clearFclBuffers(fclDeviceCtx, fclTranslationCtx, fclResult);
        return FRONTEND_BUILD_ERROR;
    }
    auto fclBuildOutput = fclResult->GetOutput(0x1);
    if (!fclBuildOutput) {
        clearFclBuffers(fclDeviceCtx, fclTranslationCtx, fclResult);
        return FRONTEND_BUILD_ERROR;
    }
    fclBuildOutput->Retain();

    auto spirvIntermediateData = reinterpret_cast<const char*>(fclBuildOutput->GetMemoryRaw());
    size_t spirvIntermediateLength = fclBuildOutput->GetSizeRaw();
    clearFclBuffers(fclDeviceCtx, fclTranslationCtx, fclResult);

    DBG_LOG("[DEBUG] FCL Success: %s (Size: %lu Bytes)\n", spirvIntermediateData, spirvIntermediateLength);

    // Backend Compilation
    igcDeviceCtx = getIgcDeviceCtx();
    if (!igcDeviceCtx) {
        return BACKEND_BUILD_ERROR;
    }
    auto idsBuffer = createIgcBuffer(context->igcMain, nullptr, 0);
    auto valuesBuffer = createIgcBuffer(context->igcMain, nullptr, 0);
    uint64_t igcTranslationCtxVersion = 0x3;

    auto igcTranslationCtx = igcDeviceCtx->CreateTranslationCtx(igcTranslationCtxVersion, codeType::spirV, codeType::oclGenBin);
    if (!igcTranslationCtx) {
        clearIgcBuffers(idsBuffer, valuesBuffer, nullptr, nullptr);
        return BACKEND_BUILD_ERROR;
    }
    auto igcResult = igcTranslationCtx->Translate(0x1, fclBuildOutput, idsBuffer, valuesBuffer, buildOptionsBuffer, internalOptionsBuffer, nullptr, 0, nullptr);
    if (!igcResult) {
        clearIgcBuffers(idsBuffer, valuesBuffer, igcTranslationCtx, nullptr);
        return BACKEND_BUILD_ERROR;
    }
    if (igcResult->Successful() == false) {
        IgcBuffer* igcBuildLog = igcResult->GetBuildLog(0x1);
        const char* igcBuildLogMem = reinterpret_cast<const char*>(igcBuildLog->GetMemoryRaw());
        if (igcBuildLogMem)
            DBG_LOG("[DEBUG] IGC build log:\n%s\n", igcBuildLogMem);
        clearIgcBuffers(idsBuffer, valuesBuffer, igcTranslationCtx, igcResult);
        return BACKEND_BUILD_ERROR;
    }
    this->igcBuildOutput = igcResult->GetOutput(0x1);
    if (!igcBuildOutput) {
        clearIgcBuffers(idsBuffer, valuesBuffer, igcTranslationCtx, igcResult);
        return BACKEND_BUILD_ERROR;
    }
    igcBuildOutput->Retain();

    this->deviceBinary = reinterpret_cast<const char*>(igcBuildOutput->GetMemoryRaw());
    size_t deviceBinarySize = igcBuildOutput->GetSizeRaw();
    clearIgcBuffers(idsBuffer, valuesBuffer, igcTranslationCtx, igcResult);

    DBG_LOG("[DEBUG] IGC Success: %s (Size: %lu Bytes)\n", deviceBinary, deviceBinarySize);

    fclBuildOutput->Release();
    buildOptionsBuffer->Release();
    internalOptionsBuffer->Release();
    programSourceBuffer->Release();

    return SUCCESS;
}

void Kernel::clearIgcBuffers(IgcBuffer* ids, IgcBuffer* values, IgcOclTranslationCtx* translationCtx, OclTranslationOutput* output) {
    if (output)
        output->Release();
    if (translationCtx)
        translationCtx->Release();
    ids->Release();
    values->Release();
}

void Kernel::clearFclBuffers(FclOclDeviceCtx* deviceCtx, FclOclTranslationCtx* translationCtx, OclTranslationOutput* output) {
    if (output)
        output->Release();
    if (translationCtx)
        translationCtx->Release();
    deviceCtx->Release();
}

int Kernel::retrieveSystemRoutineInstructions() {
    if (!igcDeviceCtx) {
        igcDeviceCtx = getIgcDeviceCtx();
        if (!igcDeviceCtx)
            return SIP_ERROR;
    }
    uint64_t interfaceID = 0xfffe2429681d9502;
    uint64_t interfaceVersion = 1u;

    ICIF* RoutineBufferCtx = createInterface(context->igcMain, interfaceID, interfaceVersion);
    auto systemRoutineBuffer = static_cast<IgcBuffer*>(RoutineBufferCtx);
    if (!systemRoutineBuffer)
        return SIP_ERROR;

    ICIF* AreaBufferCtx = createInterface(context->igcMain, interfaceID, interfaceVersion);
    auto stateSaveAreaBuffer = static_cast<IgcBuffer*>(AreaBufferCtx);
    if (!stateSaveAreaBuffer) {
        systemRoutineBuffer->Release();
        return SIP_ERROR;
    }
    uint64_t typeOfSystemRoutine = 0xfffffffffffc642;
    bool result = igcDeviceCtx->GetSystemRoutine(typeOfSystemRoutine, false, systemRoutineBuffer, stateSaveAreaBuffer);
    if (!result) {
        clearSystemRoutineBuffers(systemRoutineBuffer, stateSaveAreaBuffer);
        return SIP_ERROR;
    }
    size_t sipSize = systemRoutineBuffer->GetSizeRaw();
    const char* sipBinaryRaw = static_cast<const char*>(systemRoutineBuffer->GetMemoryRaw());
    if (!sipSize || !sipBinaryRaw) {
        clearSystemRoutineBuffers(systemRoutineBuffer, stateSaveAreaBuffer);
        return SIP_ERROR;
    }
    int ret = context->createSipAllocation(sipSize, sipBinaryRaw);
    if (ret) {
        clearSystemRoutineBuffers(systemRoutineBuffer, stateSaveAreaBuffer);
        return ret;
    }
    clearSystemRoutineBuffers(systemRoutineBuffer, stateSaveAreaBuffer);
    return SUCCESS;
}

void Kernel::clearSystemRoutineBuffers(IgcBuffer* systemRoutine, IgcBuffer* stateSaveAreaHeader) {
    systemRoutine->Release();
    stateSaveAreaHeader->Release();
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
    case PATCH_TOKEN_THREAD_PAYLOAD:
        kernelData->threadPayload = reinterpret_cast<const PatchThreadPayload*>(token);
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
    uint32_t mocsIndex = context->getMocsIndex();
    surfaceState->Bitfield.MemoryObjectControlState_Reserved = mocsIndex; // leads to data loss, I don't know why this is necessary
    surfaceState->Bitfield.MemoryObjectControlState_IndexToMocsTables = (mocsIndex >> 1);
    surfaceState->Bitfield.SurfaceBaseAddress = bufferObject->gpuAddress;

    execData.push_back(bufferObject);

    return SUCCESS;
}


int Kernel::extractMetadata() {
    // The following usage of reinterpret_cast could lead to undefined behaviour. Checking the header magic
    // makes sure that the reinterpreted memory has the correct format
    const ProgramBinaryHeader* binHeader = reinterpret_cast<const ProgramBinaryHeader*>(deviceBinary);
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
    DBG_LOG("[DEBUG] Binding Table States: %u\n", kernelData.bindingTableState->Count);
    if (unsupportedKernelArgs || hasBindlessMode)
        return INVALID_KERNEL_FORMAT;
    //TODO: Check if GROMACS uses implicit args
    uint32_t crossThreadDataSize = kernelData.dataParameterStream->DataParameterStreamSize;
    if (crossThreadDataSize) {
        crossThreadData = std::make_unique<char[]>(crossThreadDataSize);
        memset(crossThreadData.get(), 0x0, crossThreadDataSize);
    }
    uint32_t sshSize = kernelData.header->SurfaceStateHeapSize;
    if (sshSize) {
        sshLocal = std::make_unique<char[]>(sshSize);
        memcpy(sshLocal.get(), kernelData.surfaceState, sshSize);
    }
    DBG_LOG("[DEBUG] Processing Patchtokens successful!\n");
    return SUCCESS;
}

int Kernel::dumpBinary() {
    //TODO: Add chipsetID and source file name to binary name
    //TODO: Clear file if it already exists
    std::string filename = "/tmp/kernel.isabin";
    FILE* file = fopen(filename.c_str(), "w");
    if (!file)
        return KERNEL_DUMP_ERROR;
    size_t kernelSize = kernelData.header->KernelHeapSize;
    size_t bytesWritten = fwrite(kernelData.isa, 1, kernelSize, file);
    if (bytesWritten != kernelSize)
        return KERNEL_DUMP_ERROR;

    //TODO: After this, disassemble with "./iga64 -d -p "GEN9" kernel.isabin"
    return SUCCESS;
}





