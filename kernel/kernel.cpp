#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <dlfcn.h>
#include <utility>
//#include <igc/cif/cif/builtins/memory/buffer/buffer.h>
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

void createReabableSource(struct Kernel* kernel, const char* string, uint64_t* length) {
    kernel->sizeInBytes = strlen(string);
    kernel->combinedString = string;
}

// Objects needed:
// - inSrc, fclOptions, fclInternalOptions, fclTranslationCtx
// - srcCodeType & intermediateCodeType for fclTranslationCtx

int build(struct Kernel* kernel) {
    int ret;
    struct TranslationInput inputArgs = {};
    inputArgs.src_begIt = kernel->combinedString.c_str();
    inputArgs.src_endIt = kernel->combinedString.c_str() + kernel->sizeInBytes;
    inputArgs.apiOptions_begIt = kernel->options.c_str();
    inputArgs.apiOptions_endIt = kernel->options.c_str() + kernel->optionsSize;

    auto compilerInfo = (CompilerInfo*)malloc(sizeof(CompilerInfo));
    ret = loadCompiler(compilerInfo->libName, compilerInfo->igcLib, compilerInfo->igcMain);
    if (ret) {
        return ret;
    }
    kernel->compilerInfo = compilerInfo;

    //const char* size = inputArgs.src_endIt - inputArgs.src_begIt;
    //auto inSrc = CIF::Builtins::CreateConstBuffer(igcMain, inputArgs.src_begIt, size);
    return ret;
}

void* getProcAddress(void* handle, const std::string& procName) {
    void* addr = dlsym(handle, procName.c_str());
    return addr;
}

int loadCompiler(const char* libName, void* outLib, CIF::RAII::UPtr_t<CIF::CIFMain>& outlibMain) {
    struct OsLibrary ptr = {};
    auto dlopenFlag = RTLD_LAZY | RTLD_DEEPBIND;

    ptr.handle = dlopen(libName, dlopenFlag);
    if (!ptr.handle) {
        printf("Loading IGC library not successful\n");
        return COMPILER_LOAD_FAILED;
    }
    CIF::CreateCIFMainFunc_t createMain = reinterpret_cast<CIF::CreateCIFMainFunc_t>(getProcAddress(ptr.handle, CIF::CreateCIFMainFuncName));
    if (createMain == nullptr) {
        printf("Invalid compiler library\n");
        return COMPILER_LOAD_FAILED;
    }
    CIF::RAII::UPtr_t<CIF::CIFMain> main = CIF::RAII::UPtr(createMain());
    if (main == nullptr) {
        printf("Could not create main entry point\n");
        return COMPILER_LOAD_FAILED;
    }
    outLib = ptr.handle;
    outlibMain = std::move(main);
    return 0;
}

int main() {
    uint64_t sizeSource;
    auto kernel = (Kernel*)malloc(sizeof(Kernel));
    const char* raw_text = loadProgramSource("matmul.cl", &sizeSource);
    const char* program = createReadableSource(kernel, raw_text, &sizeSource);
    //cl_program program = clCreateProgramWithSource(context, 1, &raw_text, 0, &err);
    //printf("err: %d\n", err);

    std::string build_options = "-DTILE_SIZE_M=" + std::to_string(TILE_SIZE_M)
                              + " -DTILE_GROUP_M=" + std::to_string(TILE_GROUP_M)
                              + " -DTILE_SIZE_N=" + std::to_string(TILE_SIZE_N)
                              + " -DTILE_GROUP_N=" + std::to_string(TILE_GROUP_N);
    kernel->options = build_options;
    kernel->optionsSize = strlen(options);
    build(kernel);
    return 0;
}
