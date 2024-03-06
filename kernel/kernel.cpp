#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <dlfcn.h>
#include <utility>
#include "cif/builtins/memory/buffer/buffer.h"
#include "cif/common/library_api.h"
#include "cif/common/cif.h"
#include "cif/common/cif_main.h"

#include "kernel.h"

#define TILE_SIZE_M     1
#define TILE_GROUP_M    16
#define TILE_SIZE_N     128
#define TILE_GROUP_N    1

const char* loadProgramSource(const char* filename, uint64_t* _size) {
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

    *_size = size+1;
    return (const char*)source;
}

void createReadableSource(struct Kernel* kernel, const char* string, uint64_t* length) {
    kernel->sizeInBytes = strlen(string);
    kernel->combinedString = string;
}

// Objects needed:
// - inSrc, fclOptions, fclInternalOptions, fclTranslationCtx
// - srcCodeType & intermediateCodeType for fclTranslationCtx

int build(struct Kernel* kernel) {
    int ret;
    struct TranslationInput inputArgs;

    initInternalOptions(kernel);
    inputArgs.src_begIt = kernel->combinedString.c_str();
    inputArgs.src_endIt = kernel->combinedString.c_str() + kernel->sizeInBytes;
    inputArgs.apiOptions_begIt = kernel->options.c_str();
    inputArgs.apiOptions_endIt = kernel->options.c_str() + kernel->optionsSize;
    inputArgs.internalOptions_begIt = kernel->internalOptions.c_str();
    inputArgs.internalOptions_endIt = kernel->internalOptions.c_str() + kernel->internalOptionsSize;

    auto compilerInfo = (CompilerInfo*)malloc(sizeof(CompilerInfo));
    compilerInfo->libName = "libigc.so.1";
    ret = loadCompiler(compilerInfo);
    if (ret) {
        return ret;
    }
    printf("ret: %d\n", ret);
    kernel->compilerInfo = compilerInfo;

    size_t size1 = inputArgs.src_endIt - inputArgs.src_begIt;
    auto inSrc = CIF::Builtins::CreateConstBuffer(compilerInfo->igcMain.get(), inputArgs.src_begIt, size1);
    size_t size2 = inputArgs.apiOptions_endIt - inputArgs.apiOptions_begIt;
    auto fclOptions = CIF::Builtins::CreateConstBuffer(compilerInfo->igcMain.get(), inputArgs.apiOptions_begIt, size2);
    size_t size3 = inputArgs.internalOptions_endIt - inputArgs.internalOptions.begIt;
    auto fclInternalOptions = CIF::Builtins::CreateConstBuffer(igcMain.get(), inputArgs.internalOptions_begIt, size3);

    // arguments to function are missing
    auto fclTranslationCtx = createFclTranslationCtx();
    // implement the translate function
    auto fclOutput = fclTranslationCtx.get()->Translate(inSrc.get(), fclOptions.get(), fclInternalOptions.get(), nullptr, 0);

    return ret;
}

CIF::RAII::UPtr_t<IGC::FclOclTranslationCtxTagOCL> createFclTranslationCtx() {
    // look into spinlock
    std::unique_lock<std::mutex>(spinlock);

    // fclMain is still missing
    auto newDeviceCtx = fclMain->CreateInterface<IGC::FclOclDeviceCtxTagOCL>();
    if (newDeviceCtx == nullptr) {
        return nullptr;
    }
    // query OpenCL version
    newDeviceCtx->SetOclApiVersion(openCLVersion * 10);
    // maybe handle unsupported OpenCL version
    if (newDeviceCtx->GetUnderlyingVersion() > 4U) {
        printf("Underlying OpenCL version exceeds 4.0. Not supported\n");
        return nullptr;
    }
    // fclBaseTranslationCtx is missing
    // inType and outType are missing
    if (fclBaseTranslationCtx == nullptr) {
        fclBaseTranslationCtx = newDeviceCtx->CreateTranslationCtx(inType, outType);
    }
    return newDeviceCtx->CreateTranslationCtx(inType, outType);
}

// this needs to be expanded
void initInternalOptions(struct Kernel* kernel) {
    kernel->internalOptions = "-ocl-version=300 -cl-intel-has-buffer-offset-arg";
    kernel->internalOptionsSize = kernel->internalOptions.size();
}

int loadCompiler(struct CompilerInfo* compilerInfo) {
    void* handle;
    auto dlopenFlag = RTLD_LAZY | RTLD_DEEPBIND;
    handle = dlopen(compilerInfo->libName, dlopenFlag);
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
    compilerInfo->igcLib = handle;
    compilerInfo->igcMain = std::move(main);
    return 0;
}

int gpBuildKernel(struct gpuInfo* gpuInfo, const char* source, const char* options) {
    int err;
    Kernel* kernel;
        
    kernel = (Kernel*)malloc(sizeof(Kernel));
    gpuInfo->kernel = kernel;
    loadProgramSource(kernel, source);
}

int main() {
    const char* source = "matmul.cl";
    std::string build_options = "-DTILE_SIZE_M=" + std::to_string(TILE_SIZE_M)
                              + " -DTILE_GROUP_M=" + std::to_string(TILE_GROUP_M)
                              + " -DTILE_SIZE_N=" + std::to_string(TILE_SIZE_N)
                              + " -DTILE_GROUP_N=" + std::to_string(TILE_GROUP_N);
    const char* options = build_options.c_str();
    int err = gpBuildKernel(gpuInfo, source, options);

    uint64_t sizeSource;
    auto kernel = (Kernel*)malloc(sizeof(Kernel));
    const char* raw_text = loadProgramSource("matmul.cl", &sizeSource);
    createReadableSource(kernel, raw_text, &sizeSource);

    kernel->options = build_options;
    kernel->optionsSize = strlen(build_options.c_str());
    build(kernel);
    return 0;
}
