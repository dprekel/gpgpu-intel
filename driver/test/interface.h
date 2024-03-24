
struct PlatformInfo {
    uint64_t eProductFamily;
    uint64_t ePCHProductFamily;
    uint64_t eDisplayCoreFamily;
    uint64_t eRenderCoreFamily;
    uint64_t ePlatformType;
    uint16_t usDeviceID;
    uint16_t usRevId;
    uint16_t usDeviceID_PCH;
    uint16_t usRevId_PCH;
    uint64_t eGTType;
};


// generic interface for all components
struct ICIF {
    ICIF(const ICIF &) = delete;
    ICIF &operator=(const ICIF &) = delete;
    ICIF(ICIF &&) = delete;
    ICIF *operator=(ICIF &&) = delete;
    virtual void Release() = 0;
    virtual void Retain() = 0;
    virtual uint32_t GetRefCount() const = 0;
    virtual uint64_t GetEnabledVersion() const = 0;
    virtual uint64_t GetUnderlyingVersion() const;
    virtual bool GetSupportedVersions(uint64_t intId, uint64_t &verMin, uint64_t &verMax) const;
    ICIF() = default;
    virtual ~ICIF() = default;
    virtual ICIF* CreateInterfaceImpl(uint64_t intId, uint64_t version);
};

struct CIFMain : public ICIF {
    virtual uint64_t GetBinaryVersion() const = 0;
//protected:
    //CIFMain() = default;
    virtual uint64_t FindIncompatibleImpl(uint64_t entryPointInterface, const void* handle) const = 0;
    virtual bool FindSupportedVersionsImpl(uint64_t entryPointInterface, uint64_t interfaceToFind, uint64_t &verMin, uint64_t &verMax) const;
};

struct IgcBuffer : public ICIF {
    struct Impl;
    virtual Impl* GetImpl() = 0;
    virtual const Impl* GetImpl() const = 0;
    virtual void SetAllocator(void* (*allocator)(uint64_t alloc_a), void* (*deallocator)(void* alloc_b), void* (*reallocator)(void* alloc_c, uint64_t alloc_d, uint64_t alloc_e));
    virtual void SetUnderlyingStorage(void* memory, size_t size, void* (*deallocator)(void* alloc_b));
    virtual void SetUnderlyingStorage(const void* memory, size_t size);
    virtual void* DetachAllocation();
    virtual const void* GetMemoryRaw() const;
    virtual void* GetMemoryRawWriteable();
    virtual size_t GetSizeRaw() const;
    virtual size_t GetCapacityRaw() const;
    virtual bool Resize(size_t newSize);
    virtual bool Reserve(size_t newCapacity);
    virtual void Clear();
    virtual void Deallocate();
    virtual bool AlignUp(uint32_t alignment);
    virtual bool PushBackRawBytes(const void* newData, size_t size);
    virtual bool IsConst() const;
  public:
    Impl* pImpl;
};

struct Platform : public ICIF {
    struct Impl;
    virtual Impl* GetImpl();
    virtual const Impl* GetImpl() const;
    virtual uint64_t GetProductFamily() const;
    virtual void SetProductFamily(uint64_t v);
    virtual uint64_t GetPCHProductFamily() const;
    virtual void SetPCHProductFamily(uint64_t v);
    virtual uint64_t GetDisplayCoreFamily() const;
    virtual void SetDisplayCoreFamily(uint64_t v);
    virtual uint64_t GetRenderCoreFamily() const;
    virtual void SetRenderCoreFamily(uint64_t v);
    virtual uint64_t GetPlatformType() const;
    virtual void SetPlatformType(uint64_t v);

    virtual uint16_t GetDeviceID() const;
    virtual void SetDeviceID(uint16_t v);
    virtual uint16_t GetRevId() const;
    virtual void SetRevId(uint16_t v);
    virtual uint16_t GetDeviceID_PCH() const;
    virtual void SetDeviceID_PCH(uint16_t v);
    virtual uint16_t GetRevId_PCH() const;
    virtual void SetRevId_PCH(uint16_t v);

    virtual uint16_t GetGTType() const;
    virtual void SetGTType(uint64_t v);
};

struct OclTranslationOutput : public ICIF {
    struct Impl;
    virtual Impl* GetImpl();
    virtual const Impl* GetImpl() const;
  public:
    Impl* pImpl;
};

struct IgcOclTranslationCtx : public ICIF {
    struct Impl;
    virtual Impl* GetImpl();
    virtual const Impl* GetImpl() const;
  public:
    Impl* pImpl;
};

struct FclOclTranslationCtx : public ICIF {
    struct Impl;
    virtual Impl* GetImpl();
    virtual const Impl* GetImpl() const;
    virtual OclTranslationOutput* TranslateImpl(uint64_t outVersion, IgcBuffer* src, IgcBuffer* options, IgcBuffer* internalOptions, IgcBuffer* tracingOptions, uint32_t tracingOptionsCount);
    // return values missing
    virtual GetFclOptions(IgcBuffer*);
    virtual GetFclInternalOptions(IgcBuffer*);
  public:
    Impl* pImpl;
};

struct IgcOclDeviceCtx : public ICIF {
    struct Impl;
    virtual Impl* GetImpl();
    virtual const Impl* GetImpl() const;
    virtual void SetProfilingTimerResolution(float v);
    virtual Platform* GetPlatformHandleImpl(uint64_t ver);
    virtual GTSystemInfo* GetGTSystemInfoHandleImpl(uint64_t version);
    virtual IgcFeaturesAndWorkarounds* GetIgcFeaturesAndWorkaroundsHandleImpl(uint64_t version);
    virtual IgcOclTranslationCtx* CreateTranslationCtxImpl(uint64_t version, uint64_t inType, uint64_t outType);
    virtual bool GetSystemRoutine(uint64_t typeOfSystemRoutine, bool bindless, IgcBuffer* outSystemRoutineBuffer, IgcBuffer* stateSaveAreaHeaderInit);
    virtual const char* GetIGCRevision();
  public:
    Impl* pImpl;
};

struct FclOclDeviceCtx : public ICIF {
    struct Impl;
    virtual Impl* GetImpl();
    virtual const Impl* GetImpl() const;
    virtual void SetOclApiVersion(uint32_t version);
    virtual FclOclTranslationCtx* CreateTranslationCtxImpl(uint64_t ver, uint64_t inType, uint64_t outType);
    virtual uint64_t GetPreferredIntermediateRepresentation();
    virtual FclOclTranslationCtx* CreateTranslationCtxImpl(uint64_t ver, uint64_t inType, uint64_t outType, IgcBuffer* err);
    virtual Platform* GetPlatformHandleImpl(uint64_t ver);
  public:
    Impl* pImpl;
};





