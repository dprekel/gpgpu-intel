#pragma once

#include <stdio.h>

#define ADDRESS_WIDTH 48

typedef long long __m256i __attribute__((__vector_size__(32), __aligned__(32)));
typedef short __v16hi __attribute__((__vector_size__ (32)));
typedef char __v32qi __attribute__((__vector_size__ (32)));

inline __m256i _mm256_set_epi16(short w15, short w14, short w13, short w12,
                                short w11, short w10, short w09, short w08,
                                short w07, short w06, short w05, short w04,
                                short w03, short w02, short w01, short w00) {
    return __extension__(__m256i)(__v16hi){ w00, w01, w02, w03, w04, w05, w06, w07, w08, w09, w10, w11, w12, w13, w14, w15 };
}

inline __m256i _mm256_set1_epi16(short _v) {
    return _mm256_set_epi16(_v, _v, _v, _v, _v, _v, _v, _v, _v, _v, _v, _v, _v, _v, _v, _v);
}

inline __m256i _mm256_blendv_epi8(__m256i __v1, __m256i __v2, __m256i __v3) {
    return (__m256i)__builtin_ia32_pblendvb256((__v32qi)__v1, (__v32qi)__v2, (__v32qi)__v3);
}

inline void __mm256_store_si256(__m256i* __v1, __m256i __v2) {
    *__v1 = __v2;
}

inline size_t alignUp(size_t before, size_t alignment) {
    size_t mask = alignment - 1;
    return (before + mask) & ~mask;
}

template <typename T>
inline T ptrOffset(T ptrBefore, size_t offset) {
    auto addrBefore = reinterpret_cast<uintptr_t>(ptrBefore);
    return reinterpret_cast<T>(addrBefore + offset);
}

inline uint64_t canonize(uint64_t address) {
    return static_cast<int64_t>(address << (64 - ADDRESS_WIDTH)) >> (64 - ADDRESS_WIDTH);
}

inline uint32_t prevPowerOfTwo(uint32_t value) {
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    return (value - (value >> 1));
}



