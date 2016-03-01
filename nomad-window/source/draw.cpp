#include "window.h"

void window_draw_t::plot(const point_t &p, uint32_t rgb) {
    uint32_t *dst = pixels_;
    int32_t x = p.x, y = p.y;
    if (x >= 0 && y >= 0 && x < width_ && y < height_)
        dst[y * width_ + x] = rgb;
}

void window_draw_t::circle(const point_t &p, int32_t radius, uint32_t rgb) {
    const int32_t x0 = p.x, y0 = p.y;

    int32_t x = radius, y = 0;
    int32_t d = 1 - x;

    while (y <= x) {
        plot(point_t{x + x0, y + y0}, rgb);
        plot(point_t{y + x0, x + y0}, rgb);
        plot(point_t{-x + x0, y + y0}, rgb);
        plot(point_t{-y + x0, x + y0}, rgb);
        plot(point_t{-x + x0, -y + y0}, rgb);
        plot(point_t{-y + x0, -x + y0}, rgb);
        plot(point_t{x + x0, -y + y0}, rgb);
        plot(point_t{y + x0, -x + y0}, rgb);
        y++;
        d += (d <= 0) ? (2 * y + 1) : (2 * (y - (--x)) + 1);
    }
}

void window_draw_t::fill(uint32_t rgb) {
    const uint32_t area = width_ * height_;
    for (uint32_t i = 0; i < area; ++i)
        pixels_[i] = rgb;
}
