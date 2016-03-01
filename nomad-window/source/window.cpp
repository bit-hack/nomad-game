#include <assert.h>

#include "window.h"
#include "../../nomad-util/source/sdl.h"

namespace {

// blit the game render buffer to the SDL window surface with a 2X upscale
void blit2x(const uint32_t *src, const uint32_t src_width,
            const uint32_t src_height, uint32_t *dst) {
    const uint32_t dst_width = src_width * 2;
    for (uint32_t y = 0; y < src_height; ++y) {
        const uint32_t *x_src = src;
        uint32_t *x_dst_a = dst;
        uint32_t *x_dst_b = dst + dst_width;
        for (uint32_t x = 0, h = 0; x < src_width; x += 1, h += 2) {
            const uint32_t rgb = x_src[x];
            x_dst_a[h + 0] = rgb;
            x_dst_b[h + 0] = rgb;
            x_dst_a[h + 1] = rgb;
            x_dst_b[h + 1] = rgb;
        }
        src += src_width;
        dst += dst_width * 2;
    }
}
} // namespace {}

struct window_t::detail_t {
    detail_t() : surface_(nullptr) {}

    SDL_Surface *surface_;
};

window_t::window_t() : detail_(new detail_t) {}

bool window_t::init(uint32_t width, uint32_t height) {
    SDL_WM_SetCaption("Nomad", nullptr);

    // create a window at 2x scale
    SDL_Surface *surface = SDL_SetVideoMode(width * 2, height * 2, 32, 0);
    if (!surface)
        return false;

    detail_->surface_ = surface;

    draw_.pixels_ = new uint32_t[width * height];
    draw_.width_ = width;
    draw_.height_ = height;

    return true;
}

void window_t::free() {
    SDL_FreeSurface(detail_->surface_);
    detail_->surface_ = nullptr;
}

bool window_t::tick() {
    // poll all of the window events
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return false;
        }
    }

    // blit our surface to the screen
    blit2x(draw_.pixels_, draw_.width_, draw_.height_,
           (uint32_t *)detail_->surface_->pixels);

    SDL_Flip(detail_->surface_);

    SDL_Delay(1);
    return true;
}
