#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <dlfcn.h>
#include <utility>

#include "compiler_interface.h"
#include "kernel.h"
#include "gpgpu.h"
#include "gpuinit.h"
#include "hwinfo/hwinfo.h"

void loadProgramSource(Kernel* kernel, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file!\n");
        exit(1);
    }
    fseek(file, 0, SEEK_END);
    uint64_t size = ftell(file);
    rewind(file);

    char* source = (char*)malloc((size+1)*sizeof(char));
    fread((void*)source, 1, size*sizeof(char), file);
    source[size] = '\0';
    fclose(file);

    kernel->fd = file;
    kernel->sourceCode = (const char*)source;
    kernel->sourceCodeSize = size+1;
}


// Objects needed:
// - inSrc, fclOptions, fclInternalOptions, fclTranslationCtx
// - srcCodeType & intermediateCodeType for fclTranslationCtx

int build(Kernel* kernel) {
    int ret;
    TranslationInput inputArgs;
    TranslationOutput output;

    initInternalOptions(kernel);
    inputArgs.srcType = IGC::CodeType::oclC;
    inputArgs.preferredIntermediateType = IGC::CodeType::oclGenBin;
    inputArgs.src_begIt = kernel->sourceCode.c_str();
    inputArgs.src_endIt = kernel->sourceCode.c_str() + kernel->sourceCodeSize;
    inputArgs.apiOptions_begIt = kernel->options.c_str();
    inputArgs.apiOptions_endIt = kernel->options.c_str() + kernel->optionsSize;
    inputArgs.internalOptions_begIt = kernel->internalOptions.c_str();
    inputArgs.internalOptions_endIt = kernel->internalOptions.c_str() + kernel->internalOptionsSize;

    auto interf = (CompilerInterface*)malloc(sizeof(CompilerInterface));

    interf->igcName = "libigc.so.1";
    interf->fclName = "libigdfcl.so.1";
    ret = loadCompiler(&interf->igcName, &interf->igcLib, &interf->igcMain);
    ret = loadCompiler(&interf->fclName, &interf->fclLib, &interf->fclMain);
    if (ret) {
        return ret;
    }
    printf("ret: %d\n", ret);
    kernel->compilerInterface = static_cast<void*>(interf);

    size_t size1 = inputArgs.src_endIt - inputArgs.src_begIt;
    auto inSrc = CIF::Builtins::CreateConstBuffer(interf->igcMain.get(), inputArgs.src_begIt, size1);
    size_t size2 = inputArgs.apiOptions_endIt - inputArgs.apiOptions_begIt;
    auto fclOptions = CIF::Builtins::CreateConstBuffer(interf->igcMain.get(), inputArgs.apiOptions_begIt, size2);
    size_t size3 = inputArgs.internalOptions_endIt - inputArgs.internalOptions_begIt;
    auto fclInternalOptions = CIF::Builtins::CreateConstBuffer(interf->igcMain.get(), inputArgs.internalOptions_begIt, size3);
    auto idsBuffer = CIF::Builtins::CreateConstBuffer(igcMain.get(), nullptr, 0);
    auto valuesBuffer = CIF::Builtins::CreateConstBuffer(igcMain.get(), nullptr, 0);

    auto descriptor = static_cast<DeviceDescriptor*>(kernel->gpuInfo->descriptor);
    auto fclTranslationCtx = createFclTranslationCtx(interf, &inputArgs, descriptor->pHwInfo);
    // Add checks
    printf("fclTranslationCtx: %p\n", fclTranslationCtx.get());
    auto fclOutput = fclTranslationCtx.get()->Translate(inSrc.get(), fclOptions.get(), fclInternalOptions.get(), nullptr, 0);
    if (fclOutput == nullptr) {
        printf("Unknown error\n");
    }
    if (fclOutput->Successful() == true) {
        printf("Build success\n");
    }

    auto igcTranslationCtx = createIgcTranslationCtx(interf, &inputArgs, descriptor->pHwInfo);
    auto igcOutput = igcTranslationCtx.get()->Translate(
    return ret;
}

CIF::RAII::UPtr_t<IGC::FclOclTranslationCtxTagOCL> createFclTranslationCtx(CompilerInterface* interf, TranslationInput* inputArgs, const HardwareInfo* hwInfo) {
    // look into spinlock
    //std::unique_lock<std::mutex>(spinlock);

    auto newDeviceCtx = interf->fclMain->CreateInterface<IGC::FclOclDeviceCtxTagOCL>();
    if (newDeviceCtx == nullptr) {
        return nullptr;
    }
    // query OpenCL version
    uint32_t openCLVersion = 30;
    newDeviceCtx->SetOclApiVersion(openCLVersion * 10);
    // maybe handle unsupported OpenCL version
    if (newDeviceCtx->GetUnderlyingVersion() > 4U) {
        auto igcPlatform = newDeviceCtx->GetPlatformHandle();
        if (nullptr == igcPlatform.get()) {
            return nullptr;
        }
        IGC::PlatformHelper::PopulateInterfaceWith(*igcPlatform, *hwInfo->platform);
    }
    inputArgs->preferredIntermediateType = 2305843009202725362;
    if (interf->fclBaseTranslationCtx == nullptr) {
        interf->fclBaseTranslationCtx = newDeviceCtx->CreateTranslationCtx(inputArgs->srcType, inputArgs->preferredIntermediateType);
    }
    printf("fclBaseTranslationCtx: %p\n", interf->fclBaseTranslationCtx.get());
    return newDeviceCtx->CreateTranslationCtx(inputArgs->srcType, inputArgs->preferredIntermediateType);
}

CIF::RAII::UPtr_t<IGC::IgcOclTranslationCtxTagOCL> createIgcTranslationCtx(CompilerInterface* interf, TranslationInput* inputArgs, const HardwareInfo* hwInfo) {
    
    auto newDeviceCtx = interf->igcMain->CreateInterface<IGC::IgcOclDeviceCtxTagOCL>();
    if (newDeviceCtx == nullptr) {
        return nullptr;
    }
    newDeviceCtx->SetProfilingTimerResolution(static_cast<float>(outProfilingTimerResolution));
    auto igcPlatform = newDeviceCtx->GetPlatformHandle();
    auto igcGetSystemInfo = newDeviceCtx->GetGTSystemInfoHandle();
    auto igcFeWa = newDeviceCtx->GetIgcFeaturesAndWorkaroundsHandle();
    if (!igcPlatform.get() || !igcGetSystemInfo.get() || !igcFeWa.get()) {
        return nullptr;
    }
    IGC::PlatformHelper::PopulateInterfaceWith(*igcPlatform, *hwInfo->platform);
    IGC::GtSysInfoHelper::PopulateInterfaceWith(*igcPlatform, *hwInfo->gtSystemInfo);
    igcFeWa.get()->SetFtrDesktop(hwInfo->featureTable.flags.ftrDesktop);
    igcFeWa.get()->SetFtrChannelSwizzlingXOREnabled(hwInfo->featureTable.flags.ftrChannelSwizzlingXOREnabled);
    igcFeWa.get()->SetFtrGtBigDie(hwInfo->featureTable.flags.ftrGtBigDie);
    igcFeWa.get()->SetFtrGtMediumDie(hwInfo->featureTable.flags.ftrGtMediumDie);
    igcFeWa.get()->SetFtrGtSmallDie(hwInfo->featureTable.flags.ftrGtSmallDie);
    igcFeWa.get()->SetFtrGT1(hwInfo->featureTable.flags.ftrGT1);
    igcFeWa.get()->SetFtrGT1_5(hwInfo->featureTable.flags.ftrGT1_5);
    igcFeWa.get()->SetFtrGT2(hwInfo->featureTable.flags.ftrGT2);
    igcFeWa.get()->SetFtrGT3(hwInfo->featureTable.flags.ftrGT3);
    igcFeWa.get()->SetFtrGT4(hwInfo->featureTable.flags.ftrGT4);
    igcFeWa.get()->SetFtrIVBM0M1Platform(hwInfo->featureTable.flags.ftrIVBM0M1Platform);
    igcFeWa.get()->SetFtrGTL(hwInfo->featureTable.flags.ftrGT1);
    igcFeWa.get()->SetFtrGTM(hwInfo->featureTable.flags.ftrGT2);
    igcFeWa.get()->SetFtrGTH(hwInfo->featureTable.flags.ftrGT3);
    igcFeWa.get()->SetFtrSGTPVSKUStrapPresent(hwInfo->featureTable.flags.ftrSGTPVSKUStrapPresent);
    igcFeWa.get()->SetFtrGTA(hwInfo->featureTable.flags.ftrGTA);
    igcFeWa.get()->SetFtrGTC(hwInfo->featureTable.flags.ftftrrGTC);
    igcFeWa.get()->SetFtrGTX(hwInfo->featureTable.flags.ftrGTX);
    igcFeWa.get()->SetFtr5Slice(hwInfo->featureTable.flags.ftr5Slice);
    igcFeWa.get()->SetFtrGpGpuMidThreadLevelPreempt(hwInfo->featureTable.flags.ftrGpGpuMidThreadLevelPreempt);
    igcFeWa.get()->SetFtrIoMmuPageFaulting(hwInfo->featureTable.flags.ftrIoMmuPageFaulting);
    igcFeWa.get()->SetFtrWddm2Svm(hwInfo->featureTable.flags.ftrWddm2Svm);
    igcFeWa.get()->SetFtrPooledEuEnabled(hwInfo->featureTable.flags.ftrPooledEuEnabled);
    igcFeWa.get()->SetFtrResourceStreamer(hwInfo->featureTable.flags.ftrResourceStreamer);

    return newDeviceCtx->CreateTranslationCtx(inputArgs->preferredIntermediateType, inputArgs->outType);
}

// this needs to be expanded
void initInternalOptions(Kernel* kernel) {
    kernel->internalOptions = "-ocl-version=300 -cl-intel-has-buffer-offset-arg";
    kernel->internalOptionsSize = kernel->internalOptions.size();
}

int loadCompiler(const char** libName, void** libHandle, CIF::RAII::UPtr_t<CIF::CIFMain>* libMain) {
    void* handle;
    auto dlopenFlag = RTLD_LAZY | RTLD_DEEPBIND;
    handle = dlopen(*libName, dlopenFlag);
    if (!handle) {
        printf("Loading IGC library not successful\n");
        return COMPILER_LOAD_FAILED;
    }
    void* addr = dlsym(handle, CIF::CreateCIFMainFuncName);
    CIF::CreateCIFMainFunc_t createMain = reinterpret_cast<CIF::CreateCIFMainFunc_t>(addr);
    if (createMain == nullptr) {
        printf("Invalid compiler library\n");
        return COMPILER_LOAD_FAILED;
    }
    CIF::RAII::UPtr_t<CIF::CIFMain> main = CIF::RAII::UPtr(createMain());
    if (main == nullptr) {
        printf("Could not create main entry point\n");
        return COMPILER_LOAD_FAILED;
    }
    *libHandle = handle;
    *libMain = std::move(main);
    return 0;
}

int gpBuildKernel(GPU* gpuInfo, const char* filename, const char* options) {
    int err;
    Kernel* kernel;
        
    kernel = (Kernel*)malloc(sizeof(Kernel));
    kernel->gpuInfo = gpuInfo;
    gpuInfo->kernel = static_cast<void*>(kernel);
    loadProgramSource(kernel, filename);

    kernel->options = options;
    kernel->optionsSize = strlen(options);
    err = build(kernel);
    return err;
}


