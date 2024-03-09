#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <dlfcn.h>
#include <utility>

#include "compiler_interface.h"
#include "kernel.h"
#include "gpgpu.h"

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

int build(struct Kernel* kernel) {
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

    auto fclTranslationCtx = createFclTranslationCtx(interf, &inputArgs);
    // Add checks
    printf("fclTranslationCtx: %p\n", fclTranslationCtx.get());
    auto fclOutput = fclTranslationCtx.get()->Translate(inSrc.get(), fclOptions.get(), fclInternalOptions.get(), nullptr, 0);
    if (fclOutput == nullptr) {
        printf("Unknown error\n");
    }
    if (fclOutput->Successful() == true) {
        printf("Build success\n");
    }

    return ret;
}

CIF::RAII::UPtr_t<IGC::FclOclTranslationCtxTagOCL> createFclTranslationCtx(CompilerInterface* interf, TranslationInput* inputArgs) {
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
    Platform* platform = (Platform*)malloc(sizeof(Platform));
    if (newDeviceCtx->GetUnderlyingVersion() > 4U) {
        auto igcPlatform = newDeviceCtx->GetPlatformHandle();
        if (nullptr == igcPlatform.get()) {
            return nullptr;
        }
        platform->eProductFamily = 18;
        platform->ePCHProductFamily = 0;
        platform->eDisplayCoreFamily = 12;
        platform->eRenderCoreFamily = 12;
        platform->ePlatformType = 0;
        platform->usDeviceID = 6439;
        platform->usRevId = 10;
        platform->usDeviceID_PCH = 0;
        platform->usRevId_PCH = 0;
        platform->eGTType = 9;
        IGC::PlatformHelper::PopulateInterfaceWith(*igcPlatform, *platform);
    }
    inputArgs->preferredIntermediateType = 2305843009202725362;
    if (interf->fclBaseTranslationCtx == nullptr) {
        interf->fclBaseTranslationCtx = newDeviceCtx->CreateTranslationCtx(inputArgs->srcType, inputArgs->preferredIntermediateType);
    }
    printf("fclBaseTranslationCtx: %p\n", interf->fclBaseTranslationCtx.get());
    return newDeviceCtx->CreateTranslationCtx(inputArgs->srcType, inputArgs->preferredIntermediateType);
}

// this needs to be expanded
void initInternalOptions(struct Kernel* kernel) {
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

int gpBuildKernel(struct gpuInfo* gpuInfo, const char* filename, const char* options) {
    int err;
    Kernel* kernel;
        
    kernel = (Kernel*)malloc(sizeof(Kernel));
    gpuInfo->kernel = static_cast<void*>(kernel);
    loadProgramSource(kernel, filename);

    kernel->options = options;
    kernel->optionsSize = strlen(options);
    err = build(kernel);
    return err;
}


