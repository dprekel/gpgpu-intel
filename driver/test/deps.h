// all dependencies

namespace CIF {


constexpr uint64_t InvalidInterface = std::numeric_limits<uint64_t>::max();

constexpr uint64_t InvalidVersion = std::numeric_limits<uint64_t>::max();
constexpr uint64_t UnknownVersion = InvalidVersion - 1;
constexpr uint64_t TraitsSpecialVersion = UnknownVersion - 1;
constexpr uint64_t BaseVersion = 0;
inline void Abort(){
    std::abort();
}

namespace RAII {

template <typename T>
struct ReleaseHelper {
    void operator() (T *handle) const {
        assert(handle != nullptr);
        handle->Release();
    }
};

template <typename T>
using UPtr_t = std::unique_ptr<T, ReleaseHelper<T>>;

template <typename T> 
static UPtr_t<T> UPtr(T *handle) {
    return UPtr_t<T>(handle);
}

}
}
