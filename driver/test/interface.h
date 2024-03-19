
namespace IGC {

//#define CIF_DECLARE_INTERFACE(NAME, ID)
template <uint64_t version = CIF::InvalidVersion>
struct FclOclDeviceCtx;

template <>
struct FclOclDeviceCtx<CIF::BaseVersion> : public CIF::NamedCIF<CIF::InterfaceIdCoder::Enc("FCL_OCL_DEVC")> {
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
using FclOclDeviceCtxBase = FclOclDeviceCtx<CIF::BaseVersion>;
using FclOclDeviceCtxTagOCL = FclOclDeviceCtx<FclOclDeviceCtx<CIF::TraitsSpecialVersion>::GetLatestSupportedVersion()>; 


//CIF_DECLARE_INTERFACE(FclOclDeviceCtx, "FCL_OCL_DEVC");

}


namespace CIF {


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
  virtual uint64_t GetUnderlyingVersion() const {
      return GetEnabledVersion(); // by default : redirect to enabled version
  }
  virtual bool GetSupportedVersions(uint64_t intId, uint64_t &verMin,
                                    uint64_t &verMax) const {
      return false; // by default : no sub-interface are supported
  }

  // get specific version of interface
  template <typename InterfaceT, uint64_t version>
  RAII::UPtr_t<InterfaceT> CreateInterface() {
    static_assert((version >= InterfaceT::GetBackwardsCompatibilityVersion()) &&
                      (version <= InterfaceT::GetVersion()),
                  "Invalid version requested");
    return RAII::UPtr(reinterpret_cast<InterfaceT *>(
        CreateInterfaceImpl(InterfaceT::GetInterfaceId(), version)));
  }

  // get latest version of interface that matches interface class
  template <typename InterfaceT>
  RAII::UPtr_t<InterfaceT> CreateInterface() {
    uint64_t minVerSupported = 0;
    uint64_t maxVerSupported = 0;
    if (false == GetSupportedVersions(InterfaceT::GetInterfaceId(), minVerSupported, maxVerSupported)) {
      // interface not supported
      return RAII::UPtr<InterfaceT>(nullptr);
    }

    if ((InterfaceT::GetVersion() < minVerSupported) || (InterfaceT::GetVersion() > maxVerSupported)) {
      // interface version not supported
      return RAII::UPtr<InterfaceT>(nullptr);
    }

    // get latest compatible
    uint64_t chosenVersion = std::min(maxVerSupported, InterfaceT::GetVersion());

    return RAII::UPtr(reinterpret_cast<InterfaceT*>(CreateInterfaceImpl(InterfaceT::GetInterfaceId(), chosenVersion)));
  }

protected:
  ICIF() = default;
  virtual ~ICIF() = default;

  virtual ICIF *CreateInterfaceImpl(uint64_t intId, uint64_t version) {
      return nullptr; // by default : no sub-interface are supported
  }
};








struct CIFMain : public ICIF {
  virtual uint64_t GetBinaryVersion() const = 0;

  template <template <uint64_t> class InterfaceEntryPoint> bool IsCompatible(const std::vector<uint64_t> *interfacesToIgnore = nullptr) {
      return FindIncompatible<InterfaceEntryPoint>(interfacesToIgnore) == InvalidInterface;
  }

  template <template <uint64_t> class InterfaceEntryPoint>
  uint64_t FindIncompatible(const std::vector<uint64_t> *interfacesToIgnore = nullptr);
  /*
      CompatibilityEncoder encoder;
      auto encoded = encoder.Encode<InterfaceEntryPoint>(interfacesToIgnore);
      return FindIncompatibleImpl(InterfaceEntryPoint<BaseVersion>::GetInterfaceId(), encoded);
  }
  */

  template <template <uint64_t> class InterfaceEntryPoint>
  bool FindSupportedVersions(uint64_t subIntId, uint64_t &verMin, uint64_t &verMax) const {
      return FindSupportedVersionsImpl(InterfaceEntryPoint<BaseVersion>::GetInterfaceId(), subIntId, verMin, verMax);
  }

  // get latest version of builtin that matches interface class
  template <typename BuiltinT>
  RAII::UPtr_t<BuiltinT> CreateBuiltin() {
      return CreateInterface<BuiltinT>();
  }

protected:
    CIFMain() = default;

    virtual uint64_t FindIncompatibleImpl(uint64_t entryPointInterface, const void* handle) const = 0;
    virtual bool FindSupportedVersionsImpl(uint64_t entryPointInterface, uint64_t interfaceToFind, uint64_t &verMin, uint64_t &verMax) const {
        Abort(); // make pure-virtual once all mocks addapt to new interface
        return false;
    }
};

template <uint64_t id, typename BaseClass = ICIF>
struct NamedCIF : public BaseClass {
    static constexpr uint64_t GetInterfaceId() {
        return id;
    };
};




}




