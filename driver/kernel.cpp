#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <stdint.h>
#include <algorithm>
#include <cstring>

#include "kernel.h"
#include "igc_interface.h"
#include "hwinfo.h"
#include "gpgpu.h"
#include "gpuinit.h"

#define COMPILER_LOAD_FAILED -1


Kernel::Kernel(GPU* gpuInfo, const char* filename, const char* options) 
         : gpuInfo(static_cast<GPU*>(gpuInfo)),
           filename(filename),
           options(options),
           igcName("libigc.so.1"),
           fclName("libigdfcl.so.1"), 
           srcType(2305843009183132750),            // oclC
           intermediateType(2305843009202725362),   // spirV
           outType(18425635491780865102) {          // oclGenBin
    this->optionsSize = strlen(options);
}

Kernel::~Kernel() {}

int Kernel::loadProgramSource() {
    FILE* file = fopen(filename, "r");
    if (!file) {
        return -1;
    }
    fseek(file, 0, SEEK_END);
    uint64_t size = ftell(file);
    rewind(file);

    char* source = new char[size+1];
    fread(static_cast<void*>(source), 1, size*sizeof(char), file);
    source[size] = '\0';
    fclose(file);

    this->srcCode = static_cast<const char*>(source);
    this->srcSize = strlen(source+1);
    //printf("%s\n", srcCode);
    return 0;
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
    printf("Versions are ok\n");
    chosenVersion = std::min(maxVerSupported, interfaceVersion);
    
    ICIF* deviceCtx = cifMain->CreateInterfaceImpl(interfaceID, chosenVersion);
    return deviceCtx;
}

IgcBuffer* Kernel::CreateIgcBuffer(CIFMain* cifMain, const char* data, size_t size) {
    if (cifMain == nullptr) {
        return nullptr;
    }
    uint64_t interfaceID = 0xfffe2429681d9502;
    uint64_t interfaceVersion = 1;
    auto buff = CreateInterface(cifMain, interfaceID, interfaceVersion);
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
        printf("Loading compiler library not successful\n");
        return COMPILER_LOAD_FAILED;
    }
    CIFMain* (*CreateCIFMainFunc)();
    void* addr = dlsym(handle, "CIFCreateMain");
    CreateCIFMainFunc = reinterpret_cast<CIFMain*(*)()>(addr);
    if (CreateCIFMainFunc == nullptr) {
        printf("Couldn't create main entry point\n");
        return COMPILER_LOAD_FAILED;
    }
    *cifMain = CreateCIFMainFunc();
    if (*cifMain == nullptr) {
        printf("CIFCreateMain failed\n");
        return COMPILER_LOAD_FAILED;
    }
    return 0;
}

FclOclTranslationCtx* Kernel::createFclTranslationCtx() {
    uint64_t interfaceID = 95846467711642693;
    uint64_t interfaceVersion = 5;
    ICIF* DeviceCtx = CreateInterface(fclMain, interfaceID, interfaceVersion);
    FclOclDeviceCtx* newDeviceCtx = static_cast<FclOclDeviceCtx*>(DeviceCtx);
    if (newDeviceCtx == nullptr) {
        printf("No Device Context!\n");
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
        DeviceDescriptor* dd = static_cast<DeviceDescriptor*>(gpuInfo->descriptor);
        TransferPlatformInfo(igcPlatform, dd->pHwInfo->platform);
    }
    uint64_t translationCtxVersion = 1;
    auto fclTranslationCtx = newDeviceCtx->CreateTranslationCtxImpl(translationCtxVersion, srcType, intermediateType);
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
    uint64_t fam = igcPlatform->GetProductFamily();
    uint64_t core = igcPlatform->GetRenderCoreFamily();
    //printf("fam: %lu, %lu\n", fam, core);
}

void Kernel::TransferSystemInfo(GTSystemInfo* igcGetSystemInfo, SystemInfo* gtSystemInfo) {
    //igcGetSystemInfo->SetEuCount(gtSystemInfo->EUCount);
    igcGetSystemInfo->SetEuCount(48);
    uint32_t EUCount = igcGetSystemInfo->GetEUCount();
    printf("EUCount: %u\n", EUCount);
    //igcGetSystemInfo->SetThreadCount(gtSystemInfo->ThreadCount);
    igcGetSystemInfo->SetThreadCount(336);
    igcGetSystemInfo->SetSliceCount(gtSystemInfo->SliceCount);
    //igcGetSystemInfo->SetSubSliceCount(gtSystemInfo->SubSliceCount);
    igcGetSystemInfo->SetSubSliceCount(6);
    igcGetSystemInfo->SetL3CacheSizeInKb(gtSystemInfo->L3CacheSizeInKb);
    igcGetSystemInfo->SetLLCCacheSizeInKb(gtSystemInfo->LLCCacheSizeInKb);
    //igcGetSystemInfo->SetEdramSizeInKb(gtSystemInfo->EdramSizeInKb);
    igcGetSystemInfo->SetEdramSizeInKb(65536);
    igcGetSystemInfo->SetL3BankCount(gtSystemInfo->L3BankCount);
    igcGetSystemInfo->SetMaxFillRate(gtSystemInfo->MaxFillRate);
    //igcGetSystemInfo->SetEuCountPerPoolMax(gtSystemInfo->EuCountPerPoolMax);
    igcGetSystemInfo->SetEuCountPerPoolMax(0);
    //igcGetSystemInfo->SetEuCountPerPoolMin(gtSystemInfo->EuCountPerPoolMin);
    igcGetSystemInfo->SetEuCountPerPoolMin(0);
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
    uint32_t IsL3HashModeEnabled = igcGetSystemInfo->GetIsL3HashModeEnabled();
    printf("IsL3HashModeEnabled: %u\n", IsL3HashModeEnabled);
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
    igcFeWa->SetFtrIoMmuPageFaulting(featureTable->flags.ftrIoMmuPageFaulting);
    igcFeWa->SetFtrWddm2Svm(featureTable->flags.ftrWddm2Svm);
    igcFeWa->SetFtrPooledEuEnabled(featureTable->flags.ftrPooledEuEnabled);
    igcFeWa->SetFtrResourceStreamer(featureTable->flags.ftrResourceStreamer);
}

IgcOclTranslationCtx* Kernel::createIgcTranslationCtx() {
    uint64_t interfaceID = 0x15483dac4ed88c8;
    uint64_t interfaceVersion = 2;
    ICIF* DeviceCtx = CreateInterface(igcMain, interfaceID, interfaceVersion);
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
    DeviceDescriptor* dd = static_cast<DeviceDescriptor*>(gpuInfo->descriptor);
    TransferPlatformInfo(igcPlatform, dd->pHwInfo->platform);
    TransferSystemInfo(igcGetSystemInfo, dd->pHwInfo->gtSystemInfo);
    TransferFeaturesInfo(igcFeWa, dd->pHwInfo->featureTable);
    
    uint64_t translationCtxVersion = 3;
    return newDeviceCtx->CreateTranslationCtxImpl(translationCtxVersion, intermediateType, outType);
}

int Kernel::build() {
    int retFcl = loadCompiler(fclName, &fclMain);
    int retIgc = loadCompiler(igcName, &igcMain);
    if (retFcl || retIgc) {
        return -1;
    }

    IgcBuffer* src = CreateIgcBuffer(igcMain, srcCode, srcSize+1);
    IgcBuffer* buildOptions = CreateIgcBuffer(igcMain, options, optionsSize+1);
    printf("options: %s\n", options);

    const char* internal_options = "-ocl-version=300 -cl-intel-has-buffer-offset-arg -D__IMAGE_SUPPORT__=1 -fpreserve-vec3-type  -cl-ext=-all,+cl_khr_byte_addressable_store,+cl_khr_fp16,+cl_khr_global_int32_base_atomics,+cl_khr_global_int32_extended_atomics,+cl_khr_icd,+cl_khr_local_int32_base_atomics,+cl_khr_local_int32_extended_atomics,+cl_intel_command_queue_families,+cl_intel_subgroups,+cl_intel_required_subgroup_size,+cl_intel_subgroups_short,+cl_khr_spir,+cl_intel_accelerator,+cl_intel_driver_diagnostics,+cl_khr_priority_hints,+cl_khr_throttle_hints,+cl_khr_create_command_queue,+cl_intel_subgroups_char,+cl_intel_subgroups_long,+cl_khr_il_program,+cl_intel_mem_force_host_memory,+cl_khr_subgroup_extended_types,+cl_khr_subgroup_non_uniform_vote,+cl_khr_subgroup_ballot,+cl_khr_subgroup_non_uniform_arithmetic,+cl_khr_subgroup_shuffle,+cl_khr_subgroup_shuffle_relative,+cl_khr_subgroup_clustered_reduce,+cl_intel_device_attribute_query,+cl_khr_suggested_local_work_size,+cl_khr_fp64,+cl_khr_subgroups,+cl_intel_spirv_device_side_avc_motion_estimation,+cl_intel_spirv_media_block_io,+cl_intel_spirv_subgroups,+cl_khr_spirv_no_integer_wrap_decoration,+cl_intel_unified_shared_memory_preview,+cl_khr_mipmap_image,+cl_khr_mipmap_image_writes,+cl_intel_planar_yuv,+cl_intel_packed_yuv,+cl_intel_motion_estimation,+cl_intel_device_side_avc_motion_estimation,+cl_intel_advanced_motion_estimation,+cl_khr_int64_base_atomics,+cl_khr_int64_extended_atomics,+cl_khr_image2d_from_buffer,+cl_khr_depth_images,+cl_khr_3d_image_writes,+cl_intel_media_block_io,+cl_intel_va_api_media_sharing,+cl_intel_sharing_format_query,+cl_khr_pci_bus_info";
    size_t internalOptionsSize = strlen(internal_options)+1;
    IgcBuffer* internalOptions = CreateIgcBuffer(igcMain, internal_options, internalOptionsSize);
    printf("internalOptionsSize: %lu\n", internalOptionsSize);
    IgcBuffer* idsBuffer = CreateIgcBuffer(igcMain, nullptr, 0);
    IgcBuffer* valuesBuffer = CreateIgcBuffer(igcMain, nullptr, 0);

    FclOclTranslationCtx* fclTranslationCtx = createFclTranslationCtx();
    auto fclOutput = fclTranslationCtx->TranslateImpl(1, src, buildOptions, internalOptions, nullptr, 0);
    if (fclOutput == nullptr) {
        printf("FCL Compiler error!\n");
        return -1;
    }
    if (fclOutput->Successful() == true) {
        printf("Frontend build success!\n");
    }
    IgcBuffer* fclBuildLog = fclOutput->GetBuildLogImpl(1);
    const char* fclBuildLogMem = reinterpret_cast<const char*>(fclBuildLog->GetMemoryRaw());
    if (fclBuildLogMem) {
        printf("%s\n", fclBuildLogMem);
    }
    IgcBuffer* fclBuildOutput = fclOutput->GetOutputImpl(1);
    const char* fclBuildOutputMem = reinterpret_cast<const char*>(fclBuildOutput->GetMemoryRaw());
    if (fclBuildOutputMem) {
        printf("%s\n", fclBuildOutputMem);
    }
    fclBuildOutput->Retain();
    IgcOclTranslationCtx* igcTranslationCtx = createIgcTranslationCtx();
    if (igcTranslationCtx) {
        printf("igcTranslationCtx creation successful!\n");
    }
    void* gtpinInit = nullptr;
    auto igcOutput = igcTranslationCtx->TranslateImpl(1, fclBuildOutput, idsBuffer, valuesBuffer, buildOptions, internalOptions, nullptr, 0, gtpinInit);
    if (igcOutput == nullptr) {
        printf("IGC Compiler error!\n");
    }
    if (igcOutput->Successful() == true) {
        printf("Backend build successful!\n");
    }
    IgcBuffer* igcBuildLog = igcOutput->GetBuildLogImpl(1);
    const char* igcBuildLogMem = reinterpret_cast<const char*>(igcBuildLog->GetMemoryRaw());
    if (igcBuildLogMem) {
        printf("%s\n", igcBuildLogMem);
    }
    IgcBuffer* igcBuildOutput = igcOutput->GetOutputImpl(1);
    const char* igcBuildOutputMem = reinterpret_cast<const char*>(igcBuildOutput->GetMemoryRaw());
    if (igcBuildOutputMem) {
        printf("%s\n", igcBuildOutputMem);
    }

    return 0;
}

int Kernel::createKernelAllocation() {
    
}

int gpBuildKernel(GPU* gpuInfo, const char* filename, const char* options) {
    int err;
    
    Kernel* kernel = new Kernel(gpuInfo, filename, options);
    gpuInfo->kernel = static_cast<void*>(kernel);
    err = kernel->loadProgramSource();
    if (err) {
        return -1;
    }
    err = kernel->build();
    kernel->createKernelAllocation();
    return err;
}





