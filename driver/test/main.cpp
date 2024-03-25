#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <stdint.h>
#include <algorithm>

#include "interface.h"

#define COMPILER_LOAD_FAILED -1

int build(const char* fclName, const char* igcName, const char* sourceCode, size_t Codesize);
FclOclDeviceCtx* CreateInterface(CIFMain* cifMain);
IgcBuffer* CreateBuffer(CIFMain* cifMain, const void* data, size_t size);
void loadProgramSource(const char* filename, const char* sourceCode, size_t* size);

Kernel::Kernel() : 

void Kernel::loadProgramSource(const char* filename, const char* sourceCode, size_t* Codesize) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file!\n");
        return -1;
    }
    fseek(file, 0, SEEK_END);
    uint64_t size = ftell(file);
    rewind(file);

    char* source = (char*)malloc((size+1)*sizeof(char));
    fread((void*)source, 1, size*sizeof(char), file);
    source[size] = '\0';
    fclose(file);

    sourceCode = (const char*)source;
    *CodeSize = size+1;
    return 
}

ICIF* Kernel::CreateInterface(CIFMain* cifMain, uint64_t interfaceID, uint64_t interfaceVersion) {
    uint64_t chosenVersion;
    uint64_t minVerSupported = 0;
    uint64_t maxVerSupported = 0;

    bool isSupported = cifMain->GetSupportedVersions(interfaceID, minVerSupported, maxVerSupported);
    printf("maxVerSupported: %lu\n", maxVerSupported);
    printf("minVerSupported: %lu\n", minVerSupported);
    if (isSupported == false) {
        return nullptr;
    }
    if ((version < minVerSupported) || (version > maxVerSupported)) {
        return nullptr;
    }
    printf("Versions are ok\n");
    chosenVersion = std::min(maxVerSupported, version);
    
    ICIF* deviceCtx = cifMain->CreateInterfaceImpl(interfaceID, interfaceVersion);
    return deviceCtx;
}

IgcBuffer* Kernel::CreateIgcBuffer(CIFMain* cifMain, const void* data, size_t size) {
    if (cifMain == nullptr) {
        return nullptr;
    }
    uint64_t interfaceID = 0xfffe2429681d9502;
    uint64_t interfaceVersion = ;
    auto buff = CreateInterface(cifMain, interfaceID, interfaceVersion);
    IgcBuffer* buffer = static_cast<IgcBuffer*>(buff);
    if (buffer == nullptr) {
        return nullptr;
    }
    if ((data != nullptr) && (size != 0)) {
        buffer->SetUnderlyingStorage(data, size);
    }
    return buffer;
}

int Kernel::loadCompiler(const char* libName, CIFMain* cifMain) {
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
    cifMain = CreateCIFMainFunc();
    if (cifMain == nullptr) {
        printf("CIFCreateMain failed\n");
        return COMPILER_LOAD_FAILED;
    }
    return 0;
}

FclTranslationCtx* Kernel::createFclTranslationCtx(CIFMain* fclMain) {
    uint64_t interfaceID = 95846467711642693;
    uint64_t interfaceVersion = 5;
    ICIF* DeviceCtx = CreateInterface(fclMain, interfaceID, interfaceVersion);
    FclOclDeviceCtx* newDeviceCtx = static_cast<FclOclDeviceCtx*>(DeviceCtx);
    if (newDeviceCtx == nullptr) {
        printf("No Device Context!\n");
        return nullptr;
    }
    PlatformInfo platform = {18, 0, 12, 12, 0, 6439, 10, 0, 0, 9};
    uint32_t openCLVersion = 30;
    newDeviceCtx->SetOclApiVersion(openCLVersion * 10);
    if (newDeviceCtx->GetUnderlyingVersion() > 4U) {
        uint64_t platformVersion = 1;
        Platform* igcPlatform = newDeviceCtx->GetPlatformHandleImpl(platformVersion);
        if (nullptr == igcPlatform) {
            return -1;
        }
        TransferPlatformInfo(igcPlatform, hwInfo->platform);
    }
    uint64_t in = 140737353803434;
    uint64_t out = 140737488345488;
    uint64_t translationCtxVersion = 1;
    auto fclTranslationCtx = newDeviceCtx->CreateTranslationCtxImpl(translationCtxVersion, in, out);
    return fclTranslationCtx;
}

void Kernel::TransferPlatformInfo(PlatformInfo* igcPlatform, Platform* platform) {
    igcPlatform->SetProductFamily(platform.eProductFamily);
    igcPlatform->SetPCHProductFamily(platform.ePCHProductFamily);
    igcPlatform->SetDisplayCoreFamily(platform.eDisplayCoreFamily);
    igcPlatform->SetRenderCoreFamily(platform.eRenderCoreFamily);
    igcPlatform->SetPlatformType(platform.ePlatformType);
    igcPlatform->SetDeviceID(platform.usDeviceID);
    igcPlatform->SetRevId(platform.usRevId);
    igcPlatform->SetDeviceID_PCH(platform.usDeviceID_PCH);
    igcPlatform->SetRevId_PCH(platform.usRevId_PCH);
    igcPlatform->SetGTType(platform.eGTType);
    uint64_t fam = igcPlatform->GetProductFamily();
    uint64_t core = igcPlatform->GetRenderCoreFamily();
    printf("fam: %lu, %lu\n", fam, core);
}

void Kernel::TransferSystemInfo(GTSystemInfo* igcGetSystemInfo, SystemInfo* gtSystemInfo) {
    igcGetSystemInfo->SetEUCount(gtSystemInfo->EUCount);
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

IgcTranslationCtx* Kernel::createIgcTranslationCtx(CIFMain* igcMain) {
    uint64_t interfaceID = 0x15483dac4ed88c8;
    uint64_t interfaceVersion = 2;
    ICIF* DeviceCtx = CreateInterface(igcMain, interfaceID, interfaceVersion);
    IgcOclDeviceCtx* newDeviceCtx = static_cast<IgcOclDeviceCtx*>(DeviceCtx);
    if (newDeviceCtx == nullptr) {
        return nullptr;
    }
    int outProfilingTimerResolution = 83;
    newDeviceCtx->SetProfilingTimerResolution(static_cast<float>(outProfilingTimerResolution));
    uint64_t platformID = ;
    uint64_t gtsystemID = ;
    uint64_t featureID = ;
    auto igcPlatform = newDeviceCtx->GetPlatformHandleImpl();
    auto igcGetSystemInfo = newDeviceCtx->GetGTSystemInfoHandleImpl();
    auto igcFeWa = newDeviceCtx->GetIgcFeaturesAndWorkaroundsHandleImpl();
    if (!igcPlatform || !igcGetSystemInfo || !igcFeWa) {
        return nullptr;
    }
    TransferPlatformInfo(igcPlatform, hwInfo->platform);
    TransferSystemInfo(igcGetSystemInfo, hwInfo->gtSystemInfo);
    TransferFeaturesInfo(igcFeWa, hwInfo->featureTable);
    
    uint64_t translationCtxVersion = ;
    return newDeviceCtx->CreateTranslationCtx(inputArgs->preferredIntermediateType, inputArgs->outType);
}

int Kernel::build(const char* fclName, const char* igcName, const char* sourceCode, size_t codeSize) {
    int ret;
    CifMain* fclMain;
    CifMain* igcMain;

    ret = loadCompiler(fclName, fclMain);
    if (ret) {
        return -1;
    }
    ret = loadCompiler(igcName, igcMain);
    if (ret) {
        return -1;
    }

    const void* srcCode = static_cast<const void*>(sourceCode);
    IgcBuffer* src = CreateBuffer(igcMain, srcCode, codeSize);

    std::string build_options = "-DTILE_SIZE_M=" + std::to_string(TILE_SIZE_M)
                              + " -DTILE_GROUP_M=" + std::to_string(TILE_GROUP_M)
                              + " -DTILE_SIZE_N=" + std::to_string(TILE_SIZE_N)
                              + " -DTILE_GROUP_N=" + std::to_string(TILE_GROUP_N);
    size_t optionsSize = build_options.size();
    IgcBuffer* options = CreateBuffer(igcMain, build_options.c_str(), optionsSize);

    const char* internal_options = "-ocl-version=300 -cl-intel-has-buffer-offset-arg";
    size_t internalOptionsSize = strlen(internal_options);
    IgcBuffer* internalOptions = CreateBuffer(igcMain, internal_options, internalOptionsSize);

    FclOclTranslationCtx* fclTranslationCtx = createFclTranslationCtx(fclMain);
    auto fclOutput = fclTranslationCtx->TranslateImpl(140737353803434, src, options, internalOptions, 0, nullptr);
    if (fclOutput == nullptr) {
        printf("Unknown error\n");
    }
    if (fclOutput->Successful() == true) {
        printf("Frontend build success\n");
    }

    return 0;
}

int main() {
    const char* kernel = "matmul.cl";
    const char* fclName = "libigdfcl.so.1";
    const char* igcName = "libigc.so.1";
    const char* sourceCode;
    size_t CodeSize;
    loadProgramSource(kernel, sourceCode, CodeSize);
    int err = build(fclName, igcName, sourceCode, CodeSize);
    return 0;
}







