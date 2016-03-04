#pragma once
#include <stdint.h>

namespace numeric
{
namespace
{

uint64_t rand64(uint64_t & x)
{
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    return x * uint64_t(2685821657736338717llu);
}

template <typename type_t>
type_t clamp(type_t lo, type_t in, type_t hi)
{
    if (in < lo)
        return lo;
    if (in > hi)
        return hi;
    return in;
}

template <typename type_t>
type_t max2(type_t a, type_t b)
{
    return a > b ? a : b;
}

template <typename type_t>
type_t max3(type_t a, type_t b, type_t c)
{
    return max2(max2(a, b), c);
}

template <typename type_t>
type_t min2(type_t a, type_t b)
{
    return a < b ? a : b;
}

template <typename type_t>
type_t min3(type_t a, type_t b, type_t c)
{
    return min2(min2(a, b), c);
}

float lerp(float a, float b, float i)
{
    return a + (b - a) * i;
}

float randf_tri(uint64_t & x)
{
    static const uint32_t fmask = (1 << 23) - 1;
    union {
        float    f;
        uint32_t i;
    } u, v;
    u.i       = (uint32_t(rand64(x)) & fmask) | 0x3f800000;
    v.i       = (uint32_t(rand64(x)) & fmask) | 0x3f800000;
    float out = (u.f + v.f - 3.f);
    return out;
}

float randfu(uint64_t & x)
{
    static const uint32_t fmask = (1 << 23) - 1;
    union {
        float    f;
        uint32_t i;
    } u;
    u.i       = (uint32_t(rand64(x)) & fmask) | 0x3f800000;
    float out = (u.f - 1.f);
    return out;
}

float randfs(uint64_t & x)
{
    static const uint32_t fmask = (1 << 23) - 1;
    union {
        float    f;
        uint32_t i;
    } u;
    u.i       = (uint32_t(rand64(x)) & fmask) | 0x3f800000;
    float out = (u.f - 2.f);
    return out;
}

// round up to power of two
uint32_t ceil2(uint32_t n)
{
    n |= (n - 1) >> 1;
    n |= (n + 0) >> 2;
    n |= (n + 0) >> 4;
    n |= (n + 0) >> 8;
    n |= (n + 0) >> 16;
    return n + 1;
}

// wang hash
uint32_t hash(uint32_t a)
{
    a = (a ^ 61) ^ (a >> 16);
    a = a + (a << 3);
    a = a ^ (a >> 4);
    a = a * 0x27d4eb2d;
    a = a ^ (a >> 15);
    return a;
}

}  // namespace {}
}  // namespace numeric
