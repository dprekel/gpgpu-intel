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
         : context(context),
           device(context->device),
           igcMain(device->getIgcMain()),
           fclMain(device->getFclMain()),
           deviceExtensions(device->getDeviceExtensions()) {
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

KernelFromPatchtokens* Kernel::getKernelData() {
    return &kernelData;
}

BufferObject* Kernel::getConstantSurface() {
    return constantSurface.get();
}

BufferObject* Kernel::getKernelAllocation() {
    return kernelAllocation.get();
}

uint32_t Kernel::getSharedLocalMemorySize() {
    return sharedLocalMemorySize;
}

int Kernel::loadCompiler(const char* libName, CIFMain** cifMain) {
    auto dlopenFlag = RTLD_LAZY | RTLD_DEEPBIND;
    void* handle = dlopen(libName, dlopenFlag);
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

IgcBuffer* Kernel::loadProgramSource(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file)
        return nullptr;
    fseek(file, 0, SEEK_END);
    uint64_t size = ftell(file);
    rewind(file);

    source = std::make_unique<char[]>(size + 1);
    fread(source.get(), 1, size*sizeof(char), file);
    fclose(file);
    source[size] = '\0';
    //DBG_LOG("%s\n", source.get());
    auto programSourceBuffer = createIgcBuffer(igcMain, source.get(), strlen(source.get()) + 1);
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

FclOclDeviceCtx* Kernel::getFclDeviceCtx() {
    uint64_t interfaceID = 95846467711642693;
    uint64_t interfaceVersion = 0x5;
    ICIF* DeviceCtx = createInterface(fclMain, interfaceID, interfaceVersion);
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
    ICIF* DeviceCtx = createInterface(igcMain, interfaceID, interfaceVersion);
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
    fileName = filename;
    auto programSourceBuffer = loadProgramSource(filename);
    if (!programSourceBuffer)
        return SOURCE_LOAD_ERROR;
    deviceDescriptor = device->getDeviceDescriptor();
    if (!deviceDescriptor)
        return UNSUPPORTED_HARDWARE;

    const char* internalOptions = deviceExtensions.c_str();
    auto internalOptionsBuffer = createIgcBuffer(igcMain, internalOptions, strlen(internalOptions) + 1);
    size_t buildOptionsLength = buildOptions ? (strlen(buildOptions) + 1) : 0u;
    auto buildOptionsBuffer = createIgcBuffer(igcMain, buildOptions, buildOptionsLength);

    // Frontend Compilation
    auto fclDeviceCtx = getFclDeviceCtx();
    if (!fclDeviceCtx) {
        return FRONTEND_BUILD_ERROR;
    }
    uint64_t fclTranslationCtxVersion = 1u;
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
    [[maybe_unused]] auto spirvIntermediateData = reinterpret_cast<const char*>(fclBuildOutput->GetMemoryRaw());
    [[maybe_unused]] size_t spirvIntermediateLength = fclBuildOutput->GetSizeRaw();
    clearFclBuffers(fclDeviceCtx, fclTranslationCtx, fclResult);

    DBG_LOG("[DEBUG] FCL Success: %s (Size: %lu Bytes)\n", spirvIntermediateData, spirvIntermediateLength);

    // Backend Compilation
    igcDeviceCtx = getIgcDeviceCtx();
    if (!igcDeviceCtx) {
        return BACKEND_BUILD_ERROR;
    }
    auto idsBuffer = createIgcBuffer(igcMain, nullptr, 0);
    auto valuesBuffer = createIgcBuffer(igcMain, nullptr, 0);
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
    [[maybe_unused]] size_t deviceBinarySize = igcBuildOutput->GetSizeRaw();
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

    ICIF* RoutineBufferCtx = createInterface(igcMain, interfaceID, interfaceVersion);
    auto systemRoutineBuffer = static_cast<IgcBuffer*>(RoutineBufferCtx);
    if (!systemRoutineBuffer)
        return SIP_ERROR;

    ICIF* AreaBufferCtx = createInterface(igcMain, interfaceID, interfaceVersion);
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

void Kernel::decodeToken(const PatchItemHeader* token) {
    switch (token->Token) {
    case PATCH_TOKEN_BINDING_TABLE_STATE:
        kernelData.bindingTableState = reinterpret_cast<const PatchBindingTableState*>(token);
        break;
    case PATCH_TOKEN_ALLOCATE_LOCAL_SURFACE:
        kernelData.allocateLocalSurface = reinterpret_cast<const PatchAllocateLocalSurface*>(token);
        break;
    case PATCH_TOKEN_MEDIA_VFE_STATE:
        kernelData.mediaVfeState = reinterpret_cast<const PatchMediaVFEState*>(token);
        break;
    case PATCH_TOKEN_MEDIA_INTERFACE_DESCRIPTOR_LOAD:
        kernelData.mediaInterfaceDescriptorLoad = reinterpret_cast<const PatchMediaInterfaceDescriptorLoad*>(token);
        break;
    case PATCH_TOKEN_INTERFACE_DESCRIPTOR_DATA:
        kernelData.interfaceDescriptorData = reinterpret_cast<const PatchInterfaceDescriptorData*>(token);
        break;
    case PATCH_TOKEN_EXECUTION_ENVIRONMENT:
        kernelData.executionEnvironment = reinterpret_cast<const PatchExecutionEnvironment*>(token);
        break;
    case PATCH_TOKEN_KERNEL_ATTRIBUTES_INFO:
        kernelData.kernelAttributesInfo = reinterpret_cast<const PatchKernelAttributesInfo*>(token);
        break;
    case PATCH_TOKEN_THREAD_PAYLOAD:
        kernelData.threadPayload = reinterpret_cast<const PatchThreadPayload*>(token);
        break;
    case PATCH_TOKEN_DATA_PARAMETER_STREAM:
        kernelData.dataParameterStream = reinterpret_cast<const PatchDataParameterStream*>(token);
        break;
    case PATCH_TOKEN_DATA_PARAMETER_BUFFER: {
        auto tokenParam = reinterpret_cast<const PatchDataParameterBuffer*>(token);
        decodeKernelDataParameterToken(tokenParam);
        } break;
    case PATCH_TOKEN_GLOBAL_MEMORY_OBJECT_KERNEL_ARGUMENT: {
        auto tokenParam = reinterpret_cast<const PatchGlobalMemoryObjectKernelArgument*>(token);
        decodeMemoryObjectArg<const PatchGlobalMemoryObjectKernelArgument*>(tokenParam);
        } break;
    case PATCH_TOKEN_STATELESS_GLOBAL_MEMORY_OBJECT_KERNEL_ARGUMENT: {
        auto tokenParam = reinterpret_cast<const PatchStatelessGlobalMemoryObjectKernelArgument*>(token);
        decodeMemoryObjectArg<const PatchStatelessGlobalMemoryObjectKernelArgument*>(tokenParam);
        setCrossThreadDataOffset<const PatchStatelessGlobalMemoryObjectKernelArgument*>(tokenParam);
        } break;
    case PATCH_TOKEN_STATELESS_CONSTANT_MEMORY_OBJECT_KERNEL_ARGUMENT: {
        auto tokenParam = reinterpret_cast<const PatchStatelessConstantMemoryObjectKernelArgument*>(token);
        decodeMemoryObjectArg<const PatchStatelessConstantMemoryObjectKernelArgument*>(tokenParam);
        setCrossThreadDataOffset<const PatchStatelessConstantMemoryObjectKernelArgument*>(tokenParam);
        } break;
    case PATCH_TOKEN_ALLOCATE_CONSTANT_MEMORY_SURFACE_PROGRAM_BINARY_INFO: {
        kernelData.constantMemorySurface2 = reinterpret_cast<const PatchAllocateConstantMemorySurfaceProgramBinaryInfo*>(token);
        } break;
    case PATCH_TOKEN_ALLOCATE_STATELESS_CONSTANT_MEMORY_SURFACE_WITH_INITIALIZATION: {
        kernelData.constantMemorySurface = reinterpret_cast<const PatchAllocateConstantMemorySurfaceWithInitialization*>(token);
        } break;
    case PATCH_TOKEN_KERNEL_ARGUMENT_INFO:
        break;
    case PATCH_TOKEN_ALLOCATE_STATELESS_PRIVATE_MEMORY: //TODO: Check this
        break;
    case PATCH_TOKEN_STATE_SIP: // never needed
        break;
    case PATCH_TOKEN_ALLOCATE_SIP_SURFACE:  // never needed
        break;
    case PATCH_TOKEN_GTPIN_FREE_GRF_INFO:  // never needed
        break;
    case PATCH_TOKEN_MEDIA_VFE_STATE_SLOT1:  // only needed by Xe Graphics
        break;
    case PATCH_TOKEN_ALLOCATE_STATELESS_GLOBAL_MEMORY_SURFACE_WITH_INITIALIZATION:
        this->hasGlobalVariables = true;
        break;
    case PATCH_TOKEN_SAMPLER_STATE_ARRAY:
    case PATCH_TOKEN_SAMPLER_KERNEL_ARGUMENT:
    case PATCH_TOKEN_IMAGE_MEMORY_OBJECT_KERNEL_ARGUMENT:
    case PATCH_TOKEN_STATELESS_DEVICE_QUEUE_KERNEL_ARGUMENT:
    case PATCH_TOKEN_ALLOCATE_STATELESS_DEFAULT_DEVICE_QUEUE_SURFACE:
        this->hasSamplerOrQueueOrImageArgs = true;
        break;
    case PATCH_TOKEN_ALLOCATE_STATELESS_PRINTF_SURFACE: // needed if kernel contains printf statements
    case PATCH_TOKEN_STRING:
        this->hasKernelPrintf = true;
        break;
    case PATCH_TOKEN_PROGRAM_SYMBOL_TABLE:
    case PATCH_TOKEN_PROGRAM_RELOCATION_TABLE:
        this->needsLinkerSupport = true;
        break;
    case PATCH_TOKEN_GTPIN_INFO:  // maybe needed by some kernels
    case PATCH_TOKEN_ALLOCATE_SYNC_BUFFER:  // What is a Sync Buffer? Is it related to memory barriers?
    case PATCH_TOKEN_ALLOCATE_STATELESS_EVENT_POOL_SURFACE: // What is this?
        this->hasUnsupportedPatchtokens = true;
        break;
    default:
        break;
    }
}


template <typename T>
void Kernel::decodeMemoryObjectArg(T memObjectToken) {
    uint32_t argNum = memObjectToken->ArgumentNumber;
    if (argDescriptor.size() < argNum + 1)
        argDescriptor.resize(argNum + 1);
    auto pointerDesc = std::make_unique<ArgDescPointer>();
    pointerDesc->KernelArgHandler = &Kernel::setArgBuffer;
    pointerDesc->surfaceStateHeapOffset = memObjectToken->SurfaceStateHeapOffset;
    argDescriptor[argNum] = std::move(pointerDesc);
}

template <typename T>
void Kernel::setCrossThreadDataOffset(T memObjectToken) {
    uint32_t argNum = memObjectToken->ArgumentNumber;
    auto pointerDesc = static_cast<ArgDescPointer*>(argDescriptor[argNum].get());
    pointerDesc->crossThreadDataOffset = memObjectToken->DataParamOffset;
}


void Kernel::decodeKernelDataParameterToken(const PatchDataParameterBuffer* token) {
    uint32_t argNum = token->ArgumentNumber;
    switch (token->Type) {
    case DATA_PARAMETER_SUM_OF_LOCAL_MEMORY_OBJECT_ARGUMENT_SIZES: {
        if (argDescriptor.size() < argNum + 1)
            argDescriptor.resize(argNum + 1);
        auto pointerDesc = std::make_unique<ArgDescPointer>();
        pointerDesc->KernelArgHandler = &Kernel::setArgLocal;
        pointerDesc->crossThreadDataOffset = token->Offset;
        pointerDesc->requiredSlmAlignment = token->SourceOffset;
        pointerDesc->isLocal = true;
        argDescriptor[argNum] = std::move(pointerDesc);
        } break;
    case DATA_PARAMETER_KERNEL_ARGUMENT: {
        if (argDescriptor.size() < argNum + 1)
            argDescriptor.resize(argNum + 1);
        if (!argDescriptor[argNum]) {
            auto valueDesc = std::make_unique<ArgDescValue>();
            valueDesc->KernelArgHandler = &Kernel::setArgImmediate;
            argDescriptor[argNum] = std::move(valueDesc);
        }
        Element elem;
        elem.size = token->DataSize;
        elem.crossThreadDataOffset = token->Offset;
        elem.sourceOffset = token->SourceOffset;
        auto argDescValue = static_cast<ArgDescValue*>(argDescriptor[argNum].get());
        argDescValue->elements.push_back(elem);
        } break;
    case DATA_PARAMETER_BUFFER_STATEFUL:
        break;
    case DATA_PARAMETER_LOCAL_WORK_SIZE: {
        uint32_t index = token->SourceOffset >> 2;
        if (index <= 2) {
            if (!kernelData.crossThreadPayload.localWorkSize[index]) {
                kernelData.crossThreadPayload.localWorkSize[index] = token;
            } else {
                kernelData.crossThreadPayload.localWorkSize2[index] = token;
            }
        }
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
    default:
        break;
    }
}


bool Kernel::validatePatchtokens() const {
    bool valid = true;
    DBG_LOG("[DEBUG] Binding Table States: %u\n", kernelData.bindingTableState->Count);
    bool hasEssentialTokens = kernelData.bindingTableState && kernelData.executionEnvironment &&
                              kernelData.dataParameterStream && kernelData.threadPayload;
    if (kernelData.executionEnvironment->UseBindlessMode) {
        DBG_LOG("[DEBUG] Error: Kernel needs bindless mode!\n");
    }
    if (!hasEssentialTokens) {
        DBG_LOG("[DEBUG] Error: Necessary patchtokens missing!\n");
        valid = false;
    }
    if (hasGlobalVariables) {
        DBG_LOG("[DEBUG] Error: Kernel contains global variables or global constants!\n");
        valid = false;
    }
    if (hasSamplerOrQueueOrImageArgs) {
        DBG_LOG("[DEBUG] Error: Kernel has sampler, queue or image arguments!\n");
        valid = false;
    }
    if (hasKernelPrintf) {
        DBG_LOG("[DEBUG] Error: Kernel contains printf statements!\n");
        valid = false;
    }
    if (needsLinkerSupport) {
        DBG_LOG("[DEBUG] Error: Kernel needs Linker support!\n");
        valid = false;
    }        
    if (hasUnsupportedPatchtokens) {
        DBG_LOG("[DEBUG] Error: Kernel has unsupported Patchtokens!\n");
        valid = false;
    }
    return valid;
}

void Kernel::decodePatchtokensList1(const uint8_t* decodePos, const uint8_t* decodeEnd) {
    while (static_cast<uint64_t>(decodeEnd - decodePos) > sizeof(PatchItemHeader)) {
        const PatchItemHeader* token = reinterpret_cast<const PatchItemHeader*>(decodePos);
        if (token->Token != 41 && token->Token != 42)
            break;
        decodeToken(token);
        decodePos = decodePos + token->Size;
    }
}

void Kernel::decodePatchtokensList2(const uint8_t* decodePos, const uint8_t* decodeEnd) {
    while (static_cast<uint64_t>(decodeEnd - decodePos) > sizeof(PatchItemHeader)) {
        const PatchItemHeader* token = reinterpret_cast<const PatchItemHeader*>(decodePos);
        decodeToken(token);
        decodePos = decodePos + token->Size;
    }
}


int Kernel::extractMetadata() {
    // The following usage of reinterpret_cast could lead to undefined behaviour. Checking the 
    // header magic makes sure that the reinterpreted memory has the correct format
    const ProgramBinaryHeader* binHeader = reinterpret_cast<const ProgramBinaryHeader*>(deviceBinary);
    if (binHeader->Magic != 0x494E5443)
        return INVALID_KERNEL;
    DBG_LOG("[DEBUG] Number of Kernels: %u\n", binHeader->NumberOfKernels);
    if (binHeader->NumberOfKernels > 1)
        return INVALID_KERNEL;
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
    // Decode global patchtokens
    decodePatchtokensList1(patchListBlob, kernelInfoBlob);
    // Decode kernel specific patchtokens
    decodePatchtokensList2(kernelData.patchList, kernelData.patchListEnd);
    if (!validatePatchtokens())
        return UNSUPPORTED_PATCHTOKENS;
    DBG_LOG("[DEBUG] Processing Patchtokens successful!\n");

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
    this->sharedLocalMemorySize = (kernelData.allocateLocalSurface) ? kernelData.allocateLocalSurface->TotalInlineLocalMemorySize : 0u;
    // Allocate constant surface
    int ret = allocateConstantSurface();
    if (ret)
        return ret;
    // Allocate kernel BO
    size_t kernelISASize = static_cast<size_t>(kernelData.header->KernelHeapSize);
    size_t alignedAllocationSize = alignUp(kernelISASize, MemoryConstants::pageSize);
    kernelAllocation = context->allocateBufferObject(alignedAllocationSize, BufferType::KERNEL_ISA);
    if (!kernelAllocation)
        return BUFFER_ALLOCATION_FAILED;
    memcpy(kernelAllocation->cpuAddress, kernelData.isa, kernelISASize);

    return SUCCESS;
}

int Kernel::allocateConstantSurface() {
    size_t constantSize = (kernelData.constantMemorySurface2) ? kernelData.constantMemorySurface2->InlineDataSize : 0u;
    if (!constantSize)
        return SUCCESS;
    size_t alignedAllocationSize = alignUp(constantSize, MemoryConstants::pageSize);
    constantSurface = context->allocateBufferObject(alignedAllocationSize, BufferType::CONSTANT_SURFACE);
    if (!constantSurface)
        return BUFFER_ALLOCATION_FAILED;
    const uint8_t* constBufferSrc = ptrOffset(reinterpret_cast<const uint8_t*>(kernelData.constantMemorySurface2), sizeof(PatchAllocateConstantMemorySurfaceProgramBinaryInfo));
    memcpy(constantSurface->cpuAddress, constBufferSrc, constantSize);
    if (crossThreadData) {
        auto crossThreadDataPtr = ptrOffset(crossThreadData.get(), kernelData.constantMemorySurface->CrossThreadDataOffset);
        uint64_t addressToPatch = constantSurface->gpuAddress;
        uint64_t* patchLocation = reinterpret_cast<uint64_t*>(crossThreadDataPtr);
        *patchLocation = addressToPatch;
    }
    if (sshLocal) {
        auto surfaceState = ptrOffset(sshLocal.get(), kernelData.constantMemorySurface->SurfaceStateHeapOffset);
        size_t sizeToPatch = alignUp(constantSurface->size, 4);
        setSurfaceState(surfaceState, sizeToPatch, constantSurface->gpuAddress);
    }
    return SUCCESS;
}

void Kernel::setSurfaceState(char* surfaceState, size_t bufferSize, uint64_t bufferAddress) {
    auto state = reinterpret_cast<RENDER_SURFACE_STATE*>(surfaceState);
    *state = RENDER_SURFACE_STATE::init();
    SURFACE_STATE_BUFFER_LENGTH Length = {0};
    Length.Length = static_cast<uint32_t>(bufferSize - 1);
    state->Bitfield.Width = Length.SurfaceState.Width;
    state->Bitfield.Height = Length.SurfaceState.Height;
    state->Bitfield.Depth = Length.SurfaceState.Depth;
    state->Bitfield.VerticalLineStride = 0u;
    state->Bitfield.VerticalLineStrideOffset = 0u;
    state->Bitfield.MemoryObjectControlState_IndexToMocsTables = MOCS::AggressiveCaching;
    state->Bitfield.SurfaceBaseAddress = bufferAddress;
}

int Kernel::setArgument(uint32_t argIndex, size_t argSize, void* argValue) {
    if (argIndex >= argDescriptor.size())
        return INVALID_KERNEL_ARG;
    ArgDescriptor* desc = argDescriptor[argIndex].get();
    if (!desc)
        return INVALID_KERNEL_ARG;
    if (desc->argIsSet)
        return INVALID_KERNEL_ARG;
    int ret = (this->*desc->KernelArgHandler)(argIndex, argSize, argValue);
    if (ret)
        desc->argIsSet = true;
    return ret;
}

int Kernel::setArgImmediate(uint32_t argIndex, size_t argSize, void* argValue) {
    if (argSize == 0)
        return INVALID_KERNEL_ARG;
    auto argDescValue = static_cast<ArgDescValue*>(argDescriptor[argIndex].get());
    if (!argDescValue)
        return INVALID_KERNEL_ARG;
    for (auto &element : argDescValue->elements) {
        auto pDest = ptrOffset(crossThreadData.get(), element.crossThreadDataOffset);
        auto pSrc = ptrOffset(argValue, element.sourceOffset);
        if (element.sourceOffset < argSize) {
            size_t maxBytesToCopy = argSize - element.sourceOffset;
            size_t bytesToCopy = std::min(static_cast<size_t>(element.size), maxBytesToCopy);
            memcpy(pDest, pSrc, bytesToCopy);
        }
    }
    argDescValue->argIsSet = true;
    return SUCCESS;
}


int Kernel::setArgLocal(uint32_t argIndex, size_t argSize, void* argValue) {
    auto argDescPointer = static_cast<ArgDescPointer*>(argDescriptor[argIndex].get());
    uint32_t slmOffset = *ptrOffset(crossThreadData.get(), argDescPointer->crossThreadDataOffset);
    slmOffset += static_cast<uint32_t>(argSize);

    argIndex++;
    while (argIndex < argDescriptor.size()) {
        if (argDescriptor[argIndex].get()) {
            if (!argDescriptor[argIndex]->isLocal) {
                ++argIndex;
                continue;
            }
        }
        auto nextLocalArg = static_cast<ArgDescPointer*>(argDescriptor[argIndex].get());
        slmOffset = alignUp(slmOffset, nextLocalArg->requiredSlmAlignment);
        auto patchLocation = ptrOffset(crossThreadData.get(), nextLocalArg->crossThreadDataOffset);
        *reinterpret_cast<uint32_t*>(patchLocation) = slmOffset;
        ++argIndex;
    }
    this->sharedLocalMemorySize += alignUp(slmOffset, MemoryConstants::kiloByte);
    argDescPointer->argIsSet = true;
    return SUCCESS;
}


int Kernel::setArgBuffer(uint32_t argIndex, size_t argSize, void* argValue) {
    Buffer* buffer = static_cast<Buffer*>(argValue);
    if (buffer) {
        if (buffer->magic != 0x373E5A13)
            return INVALID_KERNEL_ARG;
    }
    BufferObject* bufferObject = buffer ? buffer->getDataBuffer() : nullptr;
    auto descriptor = static_cast<ArgDescPointer*>(argDescriptor[argIndex].get());
    if (descriptor->crossThreadDataOffset) {
        uint64_t* patchLocation = reinterpret_cast<uint64_t*>(ptrOffset(crossThreadData.get(), descriptor->crossThreadDataOffset));
        *patchLocation = bufferObject ? bufferObject->gpuAddress : 0u;
    }
    auto surfaceState = ptrOffset(sshLocal.get(), descriptor->surfaceStateHeapOffset);
    size_t dataBufferSize = buffer ? alignUp(buffer->getBufferSize(), 4) : 0u;
    uint64_t gpuAddress = bufferObject ? bufferObject->gpuAddress : 0u;
    setSurfaceState(surfaceState, dataBufferSize, gpuAddress);

    if (bufferObject) {
        if (execData.size() < argIndex + 1)
            execData.resize(argIndex + 1);
        execData[argIndex] = bufferObject;
    }
    descriptor->argIsSet = true;

    return SUCCESS;
}

void Kernel::resetArguments() {
    for (size_t i = 0; i < argDescriptor.size(); i++) {
        if (argDescriptor[i]) {
            argDescriptor[i]->argIsSet = false;
        }
    }
}


int Kernel::dumpBinary() {
    std::string binaryName = "/tmp/";
    size_t lastDotPos = fileName.find_last_of(".");
    if (lastDotPos != std::string::npos) {
        binaryName += fileName.substr(0, lastDotPos);
    } else {
        binaryName += fileName;
    }
    binaryName += ".isabin";
    FILE* dumpFile = fopen(binaryName.c_str(), "w");
    if (!dumpFile)
        return KERNEL_DUMP_ERROR;
    size_t kernelSize = kernelData.header->KernelHeapSize;
    size_t bytesWritten = fwrite(kernelData.isa, 1, kernelSize, dumpFile);
    if (bytesWritten != kernelSize)
        return KERNEL_DUMP_ERROR;
    fclose(dumpFile);
    DBG_LOG("[DEBUG] Kernel binary saved!\n");
    // After this, disassemble with "./iga64 -d -p "GEN9" kernel.isabin"
    return SUCCESS;
}





