#pragma once

#include <immintrin.h>
#include <stdint.h>
#include <stdio.h>

#include "context.h"

#define ADDRESS_WIDTH 48

typedef long long __m128i __attribute__((__vector_size__(16), __may_alias__));
typedef long long __m256i __attribute__((__vector_size__(32), __aligned__(32)));
typedef long long __v4di __attribute__((__vector_size__ (32)));
typedef short __v8hi __attribute__ ((__vector_size__ (16)));
typedef short __v16hi __attribute__((__vector_size__ (32)));
typedef char __v16qi __attribute__ ((__vector_size__ (16)));
typedef char __v32qi __attribute__((__vector_size__ (32)));


template <typename Vec>
inline Vec _mm_set(short _v);

template <>
inline __m256i _mm_set<__m256i>(short _v) {
    return _mm256_set_epi16(_v, _v, _v, _v, _v, _v, _v, _v, _v, _v, _v, _v, _v, _v, _v, _v);
}

template <>
inline __m128i _mm_set<__m128i>(short _v) {
    return _mm_set1_epi16(_v);
}

inline __m256i _mm_blend(__m256i __v1, __m256i __v2, __m256i __v3) {
    return (__m256i)__builtin_ia32_pblendvb256((__v32qi)__v2, (__v32qi)__v1, (__v32qi)__v3);
}

inline __m128i _mm_blend(__m128i __v1, __m128i __v2, __m128i __v3) {
    return (__m128i)__builtin_ia32_pblendvb128((__v16qi)__v2, (__v16qi)__v1, (__v16qi)__v3);
}

inline void _mm_store(__m256i* __v1, __m256i __v2) {
    *__v1 = __v2;
}

inline void _mm_store(__m128i* __v1, __m128i __v2) {
    *__v1 = __v2;
}

template <typename Vec>
inline Vec _mm_load(const void* _p) {
    const Vec* p = reinterpret_cast<const Vec*>(_p);
    return *p;
}

inline bool test_zero(__m256i __v) {
    return _mm256_testz_si256(__v, __v);
}

inline bool test_zero(__m128i __v) {
    return _mm_testz_si128(__v, __v);
}


inline size_t alignUp(size_t before, size_t alignment) {
    size_t mask = alignment - 1;
    return (before + mask) & ~mask;
}

template <typename T>
inline T ptrOffset(T ptrBefore, size_t offset) {
    uintptr_t addrBefore = reinterpret_cast<uintptr_t>(ptrBefore);
    return reinterpret_cast<T>(addrBefore + offset);
}

inline void alignedFree(void* ptr) {
    if (ptr) {
        auto originalPtr = reinterpret_cast<char**>(ptr)[-1];
        delete[] originalPtr;
    }
}

inline uint64_t maxNBitValue(uint64_t n) {
    return ((1ull << n) - 1);
}

inline uint64_t canonize(uint64_t address) {
    return static_cast<int64_t>(address << (64 - ADDRESS_WIDTH)) >> (64 - ADDRESS_WIDTH);
}

inline uint64_t decanonize(uint64_t address) {
    return (address & maxNBitValue(ADDRESS_WIDTH));
}

inline uint32_t prevPowerOfTwo(uint32_t value) {
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    return (value - (value >> 1));
}






