#include <assert.h>
#include <array>

#include "window.h"
#include "../../nomad-util/source/sdl.h"
#include "../../nomad-util/source/log.h"

namespace
{

// blit the game render buffer to the SDL window surface with a 2X upscale
void blit2x(
    const uint32_t * src, const uint32_t src_width, const uint32_t src_height,
    uint32_t * dst)
{
    const uint32_t dst_width = src_width * 2;
    for (uint32_t y = 0; y < src_height; ++y) {
        const uint32_t * x_src   = src;
        uint32_t *       x_dst_a = dst;
        uint32_t *       x_dst_b = dst + dst_width;
        for (uint32_t x = 0, h = 0; x < src_width; x += 1, h += 2) {
            const uint32_t rgb = x_src[x];
            x_dst_a[h + 0]     = rgb;
            x_dst_b[h + 0]     = rgb;
            x_dst_a[h + 1]     = rgb;
            x_dst_b[h + 1]     = rgb;
        }
        src += src_width;
        dst += dst_width * 2;
    }
}

uint8_t key_code_conv(SDLKey sym)
{
    switch (sym) {
    case (SDLK_ESCAPE): return e_key_esc;
    case (SDLK_UP): return e_key_up;
    case (SDLK_DOWN): return e_key_down;
    case (SDLK_LEFT): return e_key_left;
    case (SDLK_RIGHT): return e_key_right;
    case (SDLK_LCTRL): return e_key_lctrl;
    case (SDLK_RCTRL): return e_key_rctrl;
    case (SDLK_LSHIFT): return e_key_lshift;
    case (SDLK_RSHIFT): return e_key_rshift;
    case (SDLK_LALT): return e_key_lalt;
    case (SDLK_RALT): return e_key_ralt;
    case (SDLK_F1): return e_key_f1;
    case (SDLK_F2): return e_key_f2;
    case (SDLK_F3): return e_key_f3;
    default:
        if (sym <= 0x7f)
            return uint8_t(sym);
        else
            return e_key_eof;
    }
}
}  // namespace {}

struct window_t::detail_t {
    detail_t()
        : surface_(nullptr)
        , num_layers_(0)
    {
        memset(layers_.data(), 0, sizeof(layers_[0]) * layers_.size());
    }

    std::array<window_layer_t *, 32> layers_;
    size_t num_layers_;

    SDL_Surface * surface_;
};

window_t::window_t()
    : detail_(new detail_t)
{
}

bool window_t::init(uint32_t width, uint32_t height)
{
    SDL_WM_SetCaption("Nomad", nullptr);

    // create a window at 2x scale
    SDL_Surface * surface = SDL_SetVideoMode(width * 2, height * 2, 32, 0);
    if (!surface) {
        LOG(log_t::e_log_window, "SDL_SetVideoMode() error");
        return false;
    }

    detail_->surface_ = surface;

    draw_.pixels_ = new uint32_t[width * height];
    draw_.width_  = width;
    draw_.height_ = height;

    return true;
}

void window_t::free()
{
    SDL_FreeSurface(detail_->surface_);
    detail_->surface_ = nullptr;
}

bool window_t::tick()
{

    // poll all of the window events
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case (SDL_QUIT): return false;

        case (SDL_KEYDOWN):
        case (SDL_KEYUP): {
            window_event_t::key_t k;
            k.down_ = event.type == SDL_KEYDOWN;
            k.key_  = key_code_conv(event.key.keysym.sym);
            window_event_t e;
            e.type_ = k.type;
            e.key_  = &k;
            dispatch_event(e);
        }
        }
    }

    // mouse event is always dispatched
    {
        window_event_t::mouse_t m;
        uint8_t                 b = SDL_GetMouseState(&m.x, &m.y);
        window_event_t          e;
        e.mouse_ = &m;
        e.type_  = m.type;
        dispatch_event(e);
    }

    // process all layers
    for (window_layer_t * layer : detail_->layers_) {
        if (!layer)
            break;
        if (layer->visible_)
            layer->on_draw(this);
    }

    // blit our surface to the screen
    blit2x(
        draw_.pixels_, draw_.width_, draw_.height_,
        (uint32_t *)detail_->surface_->pixels);
    SDL_Flip(detail_->surface_);

    // dont max out the cpu
    SDL_Delay(1);
    return true;
}

void window_t::add_layer(window_layer_t * layer)
{
    LOG(log_t::e_log_window, "");

    // insertion sort
    auto & layers = detail_->layers_;
    // iterate over entire list
    window_layer_t * hand = layer;
    for (size_t i = 0;; ++i) {
        // dont add to a full list
        assert(i < layers.size() - 1);
        // end of array
        if (!layers[i]) {
            layers[i] = hand;
            break;
        } else {
            if (hand->z_ < layers[i]->z_) {
                std::swap(hand, layers[i]);
            }
        }
    }
    ++detail_->num_layers_;
}

void window_t::dispatch_event(const window_event_t & event)
{
    size_t index = detail_->num_layers_;
    if (index) {
        do {
            window_layer_t * layer = detail_->layers_[--index];
            assert(layer);
            if (layer->visible_) {
                if (layer->on_event(this, event)) {
                    // input was consumed and should not propagate
                    break;
                }
            }
        } while (index);
    }
}
