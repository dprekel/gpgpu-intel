#define COMPILER_LOAD_FAILED    -1

struct Platform {
    uint32_t eProductFamily;
    uint32_t ePCHProductFamily;
    uint32_t eDisplayCoreFamily;
    uint32_t eRenderCoreFamily;
    uint32_t ePlatformType;
    unsigned short usDeviceID;
    unsigned short usRevId;
    unsigned short usDeviceID_PCH;
    unsigned short usRevId_PCH;
    uint32_t eGTType;
};

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
CIF::RAII::UPtr_t<IGC::FclOclTranslationCtxTagOCL> createFclTranslationCtx(struct CompilerInterface* interf, TranslationInput* inputArgs);
