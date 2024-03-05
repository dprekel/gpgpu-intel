#define COMPILER_LOAD_FAILED    -1

struct OsLibrary {
    void* handle;
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
};

struct CompilerInfo {
    const char* libName = "libigc.so.1";
    void* igcLib;
    CIF::RAII::UPtr_t<CIF::CIFMain> igcMain;
};

struct Kernel {
    int fd;
    const char* filename;
    std::string combinedString;
    size_t sizeInBytes;
    std::string options;
    size_t optionsSize;

    void* compilerInfo;
};

const char* loadProgramSource(const char* filename, uint64_t* size);
void createReabableSource(struct Kernel* kernel, const char* string, uint64_t* length);
int build(struct Kernel* kernel);
void* getProcAddress(void* handle, const std::string& procName);
int loadCompiler(const char* libName, void* outLib, CIF::RAII::UPtr_t<CIF::CIFMain>& outlibMain);
