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

void loadProgramSource(const char* filename, const char* sourceCode, size_t* Codesize) {
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

ICIF* CreateInterface(CIFMain* cifMain, uint64_t interfaceID, uint64_t interfaceVersion) {
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
    
    ICIF* deviceCtx = cifMain->CreateInterfaceImpl(interfaceID, 5);
    return deviceCtx;
}

IgcBuffer* CreateIgcBuffer(CIFMain* cifMain, const void* data, size_t size) {
    if (cifMain == nullptr) {
        return nullptr;
    }
    uint64_t interfaceID = 0xfffe2429681d9502;
    auto buff = CreateInterface(cifMain, interfaceID);
    IgcBuffer* buffer = static_cast<IgcBuffer*>(buff);
    if (buffer == nullptr) {
        return nullptr;
    }
    if ((data != nullptr) && (size != 0)) {
        buffer->SetUnderlyingStorage(data, size);
    }
    return buffer;
}

int loadCompiler(const char* libName, CIFMain* cifMain) {
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

FclTranslationCtx* createFclTranslationCtx(CIFMain* fclMain) {
    uint64_t interfaceID = 95846467711642693;
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
    uint64_t in = 140737353803434;
    uint64_t out = 140737488345488;
    uint64_t translationCtxVersion = 1;
    auto fclTranslationCtx = newDeviceCtx->CreateTranslationCtxImpl(translationCtxVersion, in, out);
    return fclTranslationCtx;
}

IgcTranslationCtx* createIgcTranslationCtx(CIFMain* igcMain) {
    uint64_t interfaceID = 0x15483dac4ed88c8;
    uint64_t interfaceVersion = 2;
    ICIF* DeviceCtx = CreateInterface(igcMain, interfaceID, interfaceVersion);
    IgcOclDeviceCtx* newDeviceCtx = static_cast<IgcOclDeviceCtx*>(DeviceCtx);
    if (newDeviceCtx == nullptr) {
        return nullptr;
    }
    int outProfilingTimerResolution = 83;
    newDeviceCtx->SetProfilingTimerResolution(static_cast<float>(outProfilingTimerResolution));
    auto igcPlatform = newDeviceCtx->GetPlatformHandleImpl();
    auto igcGetSystemInfo = newDeviceCtx->GetGTSystemInfoHandle();
    auto igcFeWa = newDeviceCtx->GetIgcFeaturesAndWorkaroundsHandle();
    if (!igcPlatform.get() || !igcGetSystemInfo.get() || !igcFeWa.get()) {
        return nullptr;
    }
    IGC::PlatformHelper::PopulateInterfaceWith(*igcPlatform, *hwInfo->platform);
    IGC::GtSysInfoHelper::PopulateInterfaceWith(*igcGetSystemInfo, *hwInfo->gtSystemInfo);
    igcFeWa.get()->SetFtrDesktop(hwInfo->featureTable->flags.ftrDesktop);
    igcFeWa.get()->SetFtrChannelSwizzlingXOREnabled(hwInfo->featureTable->flags.ftrChannelSwizzlingXOREnabled);
    igcFeWa.get()->SetFtrGtBigDie(hwInfo->featureTable->flags.ftrGtBigDie);
    igcFeWa.get()->SetFtrGtMediumDie(hwInfo->featureTable->flags.ftrGtMediumDie);
    igcFeWa.get()->SetFtrGtSmallDie(hwInfo->featureTable->flags.ftrGtSmallDie);
    igcFeWa.get()->SetFtrGT1(hwInfo->featureTable->flags.ftrGT1);
    igcFeWa.get()->SetFtrGT1_5(hwInfo->featureTable->flags.ftrGT1_5);
    igcFeWa.get()->SetFtrGT2(hwInfo->featureTable->flags.ftrGT2);
    igcFeWa.get()->SetFtrGT3(hwInfo->featureTable->flags.ftrGT3);
    igcFeWa.get()->SetFtrGT4(hwInfo->featureTable->flags.ftrGT4);
    igcFeWa.get()->SetFtrIVBM0M1Platform(hwInfo->featureTable->flags.ftrIVBM0M1Platform);
    igcFeWa.get()->SetFtrGTL(hwInfo->featureTable->flags.ftrGT1);
    igcFeWa.get()->SetFtrGTM(hwInfo->featureTable->flags.ftrGT2);
    igcFeWa.get()->SetFtrGTH(hwInfo->featureTable->flags.ftrGT3);
    igcFeWa.get()->SetFtrSGTPVSKUStrapPresent(hwInfo->featureTable->flags.ftrSGTPVSKUStrapPresent);
    igcFeWa.get()->SetFtrGTA(hwInfo->featureTable->flags.ftrGTA);
    igcFeWa.get()->SetFtrGTC(hwInfo->featureTable->flags.ftrGTC);
    igcFeWa.get()->SetFtrGTX(hwInfo->featureTable->flags.ftrGTX);
    igcFeWa.get()->SetFtr5Slice(hwInfo->featureTable->flags.ftr5Slice);
    igcFeWa.get()->SetFtrGpGpuMidThreadLevelPreempt(hwInfo->featureTable->flags.ftrGpGpuMidThreadLevelPreempt);
    igcFeWa.get()->SetFtrIoMmuPageFaulting(hwInfo->featureTable->flags.ftrIoMmuPageFaulting);
    igcFeWa.get()->SetFtrWddm2Svm(hwInfo->featureTable->flags.ftrWddm2Svm);
    igcFeWa.get()->SetFtrPooledEuEnabled(hwInfo->featureTable->flags.ftrPooledEuEnabled);
    igcFeWa.get()->SetFtrResourceStreamer(hwInfo->featureTable->flags.ftrResourceStreamer);

    return newDeviceCtx->CreateTranslationCtx(inputArgs->preferredIntermediateType, inputArgs->outType);
}

int build(const char* fclName, const char* igcName, const char* sourceCode, size_t codeSize) {
    int ret;
    CifMain* fclMain;
    CiMain* igcMain;

    ret = loadCompiler(fclName, fclMain);
    if (ret) {
        return -1;
    }
    ret = loadCompiler(igcName, igcMain);
    if (ret) {
        return -1;
    }

    const void* srcCode = static_cast<const void*>(sourceCode);
    auto src = CreateBuffer(igcMain, srcCode, codeSize);

    std::string build_options = "-DTILE_SIZE_M=" + std::to_string(TILE_SIZE_M)
                              + " -DTILE_GROUP_M=" + std::to_string(TILE_GROUP_M)
                              + " -DTILE_SIZE_N=" + std::to_string(TILE_SIZE_N)
                              + " -DTILE_GROUP_N=" + std::to_string(TILE_GROUP_N);
    size_t optionsSize = build_options.size();
    auto options = CreateBuffer(igcMain, build_options.c_str(), optionsSize);

    const char* internal_options = "-ocl-version=300 -cl-intel-has-buffer-offset-arg";
    size_t internalOptionsSize = strlen(internal_options);
    auto internalOptions = CreateBuffer(igcMain, internal_options, internalOptionsSize);

    FclOclTranslationCtx* fclTranslationCtx = createFclTranslationCtx(fclMain);
    auto fclOutput = fclTranslationCtx->TranslateImpl();

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







