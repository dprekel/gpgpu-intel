
namespace CIF {

// generic interface for all components
struct ICIF {
    //ICIF(const ICIF &) = delete;
    //ICIF &operator=(const ICIF &) = delete;
    //ICIF(ICIF &&) = delete;
    //ICIF *operator=(ICIF &&) = delete;
    virtual void Release() = 0;
    virtual void Retain() = 0;
    virtual uint32_t GetRefCount() const = 0;
    virtual uint64_t GetEnabledVersion() const = 0;
    virtual uint64_t GetUnderlyingVersion() const;
    virtual bool GetSupportedVersions(uint64_t intId, uint64_t &verMin, uint64_t &verMax) const;
    //ICIF() = default;
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


namespace Builtins {

struct Buffer {
    //virtual void SetAllocator(AllocatorT allocator, DeallocatorT deallocator, ReallocatorT reallocator);
    //virtual void SetUnderlyingStorage(void* memory, size_t size, DeallocatorT deallocator);
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
};



}

/*
template <uint64_t id, typename BaseClass = ICIF>
struct NamedCIF : public BaseClass {
    static constexpr uint64_t GetInterfaceId() {
        return id;
    };
};
*/


}


namespace IGC {


struct Platform {
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

struct OclTranslationOutput {

};

struct FclOclTranslationCtx {
    virtual OclTranslationOutput* TranslateImpl(uint64_t outVersion, CIF::Builtins::Buffer* src, CIF::Builtins::Buffer* options, CIF::Builtins::Buffer* internalOptions, CIF::Builtins::Buffer* tracingOptions, uint32_t tracingOptionsCount);
};


struct FclOclDeviceCtx : public CIF::ICIF {
    struct Impl;
    virtual Impl* GetImpl();
    virtual const Impl* GetImpl() const;
    virtual void SetOclApiVersion(uint32_t version);
    virtual FclOclTranslationCtx* CreateTranslationCtxImpl(uint64_t ver, uint64_t inType, uint64_t outType);
    virtual uint64_t GetPreferredIntermediateRepresentation();
    virtual FclOclTranslationCtx* CreateTranslationCtxImpl(uint64_t ver, uint64_t inType, uint64_t outType, CIF::Builtins::Buffer* err);
    virtual Platform* GetPlatformHandleImpl(uint64_t ver);
};




}




/*
namespace IGC {

struct NamedCIF : public ICIF {
    static constexpr uint64_t GetInterfaceId() {
        return 95846467711642693;
    };
};

//#define CIF_DECLARE_INTERFACE(NAME, ID)
template <uint64_t version = CIF::InvalidVersion>
struct FclOclDeviceCtx;

template <>
struct FclOclDeviceCtx<CIF::BaseVersion> : public NamedCIF { //<CIF::InterfaceIdCoder::Enc("FCL_OCL_DEVC")> {
    struct Impl;
    virtual Impl * GetImpl(){ 
        return pImpl;
    }
    virtual const Impl * GetImpl() const { 
        return pImpl;
    }
  protected:
    ~FclOclDeviceCtx() override;
    Impl* pImpl;
    template<typename ... ArgsT>
    FclOclDeviceCtx(ArgsT &&... args);
    FclOclDeviceCtx(Impl* pImpl);
};
//using FclOclDeviceCtxBase = FclOclDeviceCtx<CIF::BaseVersion>;
//using FclOclDeviceCtxTagOCL = FclOclDeviceCtx<FclOclDeviceCtx<CIF::TraitsSpecialVersion>::GetLatestSupportedVersion()>; 


//CIF_DECLARE_INTERFACE(FclOclDeviceCtx, "FCL_OCL_DEVC");


}
*/

