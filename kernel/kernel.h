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
    static std::mutex spinlock;

    const char* libName = "libigc.so.1";
    void* igcLib;
    CIF::RAII::UPtr_t<CIF::CIFMain> igcMain;

    void* fclLib;
    CIF::RAII::UPtr_t<CIF::CIFMain> fclMain;
};

struct Kernel {
    int fd;
    const char* filename;
    void* compilerInfo;

    std::string combinedString;
    size_t sizeInBytes;

    std::string options;
    size_t optionsSize;

    std::string internalOptions;
    size_t internalOptionsSize;
};

const char* loadProgramSource(const char* filename, uint64_t* size);
void createReabableSource(struct Kernel* kernel, const char* string, uint64_t* length);
int build(struct Kernel* kernel);
int loadCompiler(struct CompilerInfo* compilerInfo);
void initInternalOptions(struct Kernel* kernel);
