#pragma once

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

struct PlatformInfo : public ICIF {
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
  public:
    Impl* pImpl;
};

struct GTSystemInfo : public ICIF {
    struct Impl;
    virtual Impl* GetImpl();
    virtual const Impl* GetImpl() const;
    virtual uint32_t GetEUCount() const;
    virtual void SetEuCount(uint32_t v);
    virtual uint32_t GetThreadCount() const;
    virtual void SetThreadCount(uint32_t v);
    virtual uint32_t GetSliceCount() const;
    virtual void SetSliceCount(uint32_t v);
    virtual uint32_t GetSubSliceCount() const;
    virtual void SetSubSliceCount(uint32_t v);
    virtual uint32_t GetL3CacheSizeInKb() const;
    virtual void SetL3CacheSizeInKb(uint32_t v);
    virtual uint32_t GetLLCCacheSizeInKb() const;
    virtual void SetLLCCacheSizeInKb(uint32_t v);
    virtual uint32_t GetEdramSizeInKb() const;
    virtual void SetEdramSizeInKb(uint32_t v);
    virtual uint32_t GetL3BankCount() const;
    virtual void SetL3BankCount(uint32_t v);
    virtual uint32_t GetMaxFillRate() const;
    virtual void SetMaxFillRate(uint32_t v);
    virtual uint32_t GetEuCountPerPoolMax() const;
    virtual void SetEuCountPerPoolMax(uint32_t v);
    virtual uint32_t GetEuCountPerPoolMin() const;
    virtual void SetEuCountPerPoolMin(uint32_t v);
    virtual uint32_t GetTotalVsThreads() const;
    virtual void SetTotalVsThreads(uint32_t v);
    virtual uint32_t GetTotalHsThreads() const;
    virtual void SetTotalHsThreads(uint32_t v);
    virtual uint32_t GetTotalDsThreads() const;
    virtual void SetTotalDsThreads(uint32_t v);
    virtual uint32_t GetTotalGsThreads() const;
    virtual void SetTotalGsThreads(uint32_t v);
    virtual uint32_t GetTotalPsThreadsWindowerRange() const;
    virtual void SetTotalPsThreadsWindowerRange(uint32_t v);
    virtual uint32_t GetCsrSizeInMb() const;
    virtual void SetCsrSizeInMb(uint32_t v);
    virtual uint32_t GetMaxEuPerSubSlice() const;
    virtual void SetMaxEuPerSubSlice(uint32_t v);
    virtual uint32_t GetMaxSlicesSupported() const;
    virtual void SetMaxSlicesSupported(uint32_t v);
    virtual uint32_t GetMaxSubSlicesSupported() const;
    virtual void SetMaxSubSlicesSupported(uint32_t v);
    virtual uint32_t GetIsL3HashModeEnabled() const;
    virtual void SetIsL3HashModeEnabled(uint32_t v);
    virtual uint32_t GetIsDynamicallyPopulated() const;
    virtual void SetIsDynamicallyPopulated(uint32_t v);
    virtual uint32_t GetMaxDualSubSlicesSupported() const;
    virtual void SetMaxDualSubSlicesSupported(uint32_t v);
    virtual uint32_t GetDualSubSliceCount() const;
    virtual void SetDualSubSliceCount(uint32_t v);
  public:
    Impl* pImpl;
};

struct IgcFeaturesAndWorkarounds : public ICIF {
    struct Impl;
    virtual Impl* GetImpl();
    virtual const Impl* GetImpl() const;
    virtual bool GetFtrDesktop() const; 
    virtual void SetFtrDesktop(bool v);
    virtual bool GetFtrChannelSwizzlingXOREnabled() const; 
    virtual void SetFtrChannelSwizzlingXOREnabled(bool v);
    virtual bool GetFtrGtBigDie() const; 
    virtual void SetFtrGtBigDie(bool v);
    virtual bool GetFtrGtMediumDie() const; 
    virtual void SetFtrGtMediumDie(bool v);
    virtual bool GetFtrGtSmallDie() const; 
    virtual void SetFtrGtSmallDie(bool v);
    virtual bool GetFtrGT1() const; 
    virtual void SetFtrGT1 (bool v);
    virtual bool GetFtrGT1_5() const; 
    virtual void SetFtrGT1_5(bool v);
    virtual bool GetFtrGT2() const; 
    virtual void SetFtrGT2(bool v);
    virtual bool GetFtrGT3() const; 
    virtual void SetFtrGT3(bool v);
    virtual bool GetFtrGT4() const; 
    virtual void SetFtrGT4(bool v);
    virtual bool GetFtrIVBM0M1Platform() const; 
    virtual void SetFtrIVBM0M1Platform(bool v);
    virtual bool GetFtrGTL() const; 
    virtual void SetFtrGTL(bool v);
    virtual bool GetFtrGTM() const; 
    virtual void SetFtrGTM(bool v);
    virtual bool GetFtrGTH() const; 
    virtual void SetFtrGTH(bool v);
    virtual bool GetFtrSGTPVSKUStrapPresent() const; 
    virtual void SetFtrSGTPVSKUStrapPresent(bool v);
    virtual bool GetFtrGTA() const; 
    virtual void SetFtrGTA(bool v);
    virtual bool GetFtrGTC() const; 
    virtual void SetFtrGTC(bool v);
    virtual bool GetFtrGTX() const; 
    virtual void SetFtrGTX(bool v);
    virtual bool GetFtr5Slice() const; 
    virtual void SetFtr5Slice(bool v);
    virtual bool GetFtrGpGpuMidThreadLevelPreempt() const; 
    virtual void SetFtrGpGpuMidThreadLevelPreempt(bool v);
    virtual bool GetFtrIoMmuPageFaulting() const; 
    virtual void SetFtrIoMmuPageFaulting(bool v);
    virtual bool GetFtrWddm2Svm() const; 
    virtual void SetFtrWddm2Svm(bool v);
    virtual bool GetFtrPooledEuEnabled() const; 
    virtual bool SetFtrPooledEuEnabled(bool v); 
    virtual void GetFtrResourceStreamer() const;
    virtual void SetFtrResourceStreamer(bool v);
    virtual void GetMaxOCLParamSize() const;
    virtual void SetMaxOCLParamSize(bool v);
  public:
    Impl* pImpl;
};

struct OclTranslationOutput : public ICIF {
    struct Impl;
    virtual Impl* GetImpl();
    virtual const Impl* GetImpl() const;
    virtual bool Successful() const;
    virtual bool HasWarnings() const;
    virtual uint64_t GetOutputType() const;
    virtual IgcBuffer* GetBuildLogImpl(uint64_t bufferVersion);
    virtual IgcBuffer* GetOutputImpl(uint64_t bufferVersion);
    virtual IgcBuffer* GetDebugDataImpl(uint64_t bufferVersion);
  public:
    Impl* pImpl;
};

struct IgcOclTranslationCtx : public ICIF {
    struct Impl;
    virtual Impl* GetImpl();
    virtual const Impl* GetImpl() const;
    virtual OclTranslationOutput* TranslateImpl(uint64_t outVersion, IgcBuffer* src, IgcBuffer* options, IgcBuffer* internalOptions, IgcBuffer* tracingOptions, uint32_t tracingOptionsCount);
    virtual OclTranslationOutput* TranslateImpl(uint64_t outVersion, IgcBuffer* src, IgcBuffer* options, IgcBuffer* internalOptions, IgcBuffer* tracingOptions, uint32_t tracingOptionsCount, void* gtPinInput);
    virtual bool GetSpecConstantsInfoImpl(IgcBuffer* src, IgcBuffer* outSpecConstantsIds, IgcBuffer* outSpecConstantsSizes);
    virtual OclTranslationOutput* TranslateImpl(uint64_t outVersion, IgcBuffer* src, IgcBuffer* specConstantsIds, IgcBuffer* specConstantsValues, IgcBuffer* options, IgcBuffer* internalOptions, IgcBuffer* tracingOptions, uint32_t tracingOptionsCount, void* gtPinInput);
  public:
    Impl* pImpl;
};

struct FclOclTranslationCtx : public ICIF {
    struct Impl;
    virtual Impl* GetImpl();
    virtual const Impl* GetImpl() const;
    virtual OclTranslationOutput* TranslateImpl(uint64_t outVersion, IgcBuffer* src, IgcBuffer* options, IgcBuffer* internalOptions, IgcBuffer* tracingOptions, uint32_t tracingOptionsCount);
    // return values missing
    virtual void GetFclOptions(IgcBuffer*);
    virtual void GetFclInternalOptions(IgcBuffer*);
  public:
    Impl* pImpl;
};

struct IgcOclDeviceCtx : public ICIF {
    struct Impl;
    virtual Impl* GetImpl();
    virtual const Impl* GetImpl() const;
    virtual void SetProfilingTimerResolution(float v);
    virtual PlatformInfo* GetPlatformHandleImpl(uint64_t ver);
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
    virtual PlatformInfo* GetPlatformHandleImpl(uint64_t ver);
  public:
    Impl* pImpl;
};





