
template<typename DstType, typename SrcType>
UPtr_t<DstType> RetainAndPack(SrcType * ptr){
    if(ptr == nullptr){
        return UPtr<DstType>(nullptr);
    }

    ptr->Retain();
    return UPtr<DstType>(static_cast<DstType*>(ptr));
}


template<> struct FclOclDeviceCtx<4> : public CIF::VersionedCIF<FclOclDeviceCtx, 4, 3> {
    using VersionedCIF::VersionedCIF;

    template <typename PlatformInterface = PlatformTagOCL>
    CIF::RAII::UPtr_t<PlatformInterface> GetPlatformHandle() {
        return CIF::RAII::RetainAndPack<PlatformInterface>( GetPlatformHandleImpl(PlatformInterface::GetVersion()) );
    }

protected:
    virtual PlatformBase *GetPlatformHandleImpl(CIF::Version_t ver);
};
