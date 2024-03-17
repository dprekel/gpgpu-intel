#pragma once

#include "gpgpu.h"
#include "hwinfo/hwinfo.h"

#define COMPILER_LOAD_FAILED    -1

struct TranslationInput {
    bool allowCaching;
    const char* src_begIt;
    const char* src_endIt;

    const char* apiOptions_begIt;
    const char* apiOptions_endIt;

    const char* internalOptions_begIt;
    const char* internalOptions_endIt;

    const char* tracingOptions = nullptr;
    uint32_t tracingOptionsCount = 0;

    IGC::CodeType::CodeType_t srcType;
    IGC::CodeType::CodeType_t preferredIntermediateType;
    IGC::CodeType::CodeType_t outType;

    void* GTPinInput = nullptr;
};

struct TranslationOutput {
    IGC::CodeType::CodeType_t intermediateCodeType;

    std::string intermediate;
    size_t intermediateSize;

    std::string binary;
    size_t binarySize;

    std::string debugData;
    size_t debugDataSize;

    std::string frontendCompilerLog;
    std::string backendCompilerLog;
};

struct CompilerInterface {
    //static std::mutex spinlock;

    const char* igcName = "libigc.so.1";
    void* igcLib;
    CIF::RAII::UPtr_t<CIF::CIFMain> igcMain;

    const char* fclName = "libigdfcl.so.1";
    void* fclLib;
    CIF::RAII::UPtr_t<CIF::CIFMain> fclMain;
    CIF::RAII::UPtr_t<IGC::FclOclTranslationCtxTagOCL> fclBaseTranslationCtx;
};

struct Kernel {
    GPU* gpuInfo;
    FILE* fd;
    const char* filename;
    void* compilerInterface;

    std::string sourceCode;
    size_t sourceCodeSize;

    std::string options;
    size_t optionsSize;

    std::string internalOptions;
    size_t internalOptionsSize;
};

const char* loadProgramSource(const char* filename, uint64_t* size);
void createReabableSource(struct Kernel* kernel, const char* string, uint64_t* length);
int build(struct Kernel* kernel);
int loadCompiler(const char** libName, void** libHandle, CIF::RAII::UPtr_t<CIF::CIFMain>* libMain);
void initInternalOptions(struct Kernel* kernel);
CIF::RAII::UPtr_t<IGC::FclOclTranslationCtxTagOCL> createFclTranslationCtx(CompilerInterface* interf, TranslationInput* inputArgs, const HardwareInfo* hwInfo);
CIF::RAII::UPtr_t<IGC::IgcOclTranslationCtxTagOCL> createIgcTranslationCtx(CompilerInterface* interf, TranslationInput* inputArgs, const HardwareInfo* hwInfo);







