#pragma once
#include <stdint.h>

namespace geom
{

template <typename type_t>
struct vec2_t {
    type_t x, y;
};

template <typename type_t>
struct rect2_t {
    type_t x0, y0;
    type_t x1, y1;
};

template <typename type_t>
bool inside(const rect2_t<type_t> & rect, const vec2_t<type_t> & point)
{
    bool res = true;
    res &= point.x >= rect.x0;
    res &= point.y >= rect.y0;
    res &= point.x <= rect.x1;
    res &= point.y <= rect.y1;
    return res;
}

template <typename type_t>
type_t distance_sqr(const vec2_t<type_t> & a, const vec2_t<type_t> & b)
{
    type_t dx = b.x - a.x;
    type_t dy = b.y - a.y;
    return dx * dx + dy * dy;
}

template <typename type_t>
vec2_t<type_t> operator+(const vec2_t<type_t> & a, const vec2_t<type_t> & b)
{
    return vec2_t<type_t>{a.x + b.x, a.y + b.y};
}

template <typename type_t>
vec2_t<type_t> operator-(const vec2_t<type_t> & a, const vec2_t<type_t> & b)
{
    return vec2_t<type_t>{a.x - b.x, a.y - b.y};
}

template <typename type_t>
type_t operator*(const vec2_t<type_t> & a, const vec2_t<type_t> & b)
{
    return a.x * b.x + a.y * b.y;
}

typedef vec2_t<int32_t>  vec2i_t;
typedef rect2_t<int32_t> rect2i_t;

}  // namespace geom
