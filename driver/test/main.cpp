#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <memory>
#include <limits>
#include <dlfcn.h>
#include <stdint.h>
#include "deps.h"
#include "interface.h"

#define COMPILER_LOAD_FAILED -1

int loadCompiler(const char* libName) {
    void* handle;
    struct CIF::CIFMain* cifMain;
    auto dlopenFlag = RTLD_LAZY | RTLD_DEEPBIND;
    handle = dlopen(libName, dlopenFlag);
    if (!handle) {
        printf("Loading IGC library not successful\n");
        return COMPILER_LOAD_FAILED;
    }
    CIF::CIFMain* (*CreateCIFMainFunc)();
    void* addr = dlsym(handle, "CIFCreateMain");
    CreateCIFMainFunc = reinterpret_cast<CIF::CIFMain*(*)()>(addr);
    if (CreateCIFMainFunc == nullptr) {
        printf("Invalid compiler library\n");
        return COMPILER_LOAD_FAILED;
    }
    cifMain = CreateCIFMainFunc();
    if (cifMain != nullptr) {
        printf("Could create main entry point\n");
        return COMPILER_LOAD_FAILED;
    }
    auto newDeviceCtx = cifMain->CreateInterface<IGC::FclOclDeviceCtxTagOCL>();
    return 0;
}

int main() {
    const char* fclName = "libigdfcl.so.1";
    int err = loadCompiler(fclName);
    return 0;
}
