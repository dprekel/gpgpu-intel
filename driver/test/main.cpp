#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <stdint.h>
#include <algorithm>

#include "interface.h"

#define COMPILER_LOAD_FAILED -1

int loadCompiler(const char* libName);
IGC::FclOclDeviceCtx* CreateInterface(CIF::CIFMain* cifMain);

IGC::FclOclDeviceCtx* CreateInterface(CIF::CIFMain* cifMain) {
    uint64_t chosenVersion;
    uint64_t minVerSupported = 0;
    uint64_t maxVerSupported = 0;
    uint64_t interfaceID = 95846467711642693;
    uint64_t version = 5;
    bool isSupported;

    isSupported = cifMain->GetSupportedVersions(interfaceID, minVerSupported, maxVerSupported);
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
    
    CIF::ICIF* deviceCtx = cifMain->CreateInterfaceImpl(95846467711642693, 5);
    // static_cast instead of reinterpret_cast to avoid wrong vtable offsets
    return static_cast<IGC::FclOclDeviceCtx*>(deviceCtx);
}


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
        //return COMPILER_LOAD_FAILED;
    }
    IGC::FclOclDeviceCtx* newDeviceCtx = CreateInterface(cifMain);

    if (newDeviceCtx == nullptr) {
        printf("No Device Context!\n");
    }
    uint32_t openCLVersion = 30;
    newDeviceCtx->SetOclApiVersion(openCLVersion * 10);
    // maybe handle unsupported OpenCL version
    if (newDeviceCtx->GetUnderlyingVersion() > 4U) {
        /*
        auto igcPlatform = deviceCtx->GetPlatformHandle();
        if (nullptr == igcPlatform) {
            return -1;
        }
        */
        printf("ok\n");
        //IGC::PlatformHelper::PopulateInterfaceWith(*igcPlatform, *hwInfo->platform);
        //PopulateInterfaceWith(*igcPlatform, *hwInfo->platform);
    }
    return 0;
}

int main() {
    const char* fclName = "libigdfcl.so.1";
    int err = loadCompiler(fclName);
    return 0;
}



