#pragma once
#include <stdint.h>
#include <assert.h>
#include <memory>

#include "../../nomad-util/source/geometric.h"

// handles all window drawing operations
struct window_draw_t {
    uint32_t * pixels_;
    int32_t    width_;
    int32_t    height_;

    void circle(const geom::vec2i_t & pos, int32_t radius, uint32_t rgb);

    void plot(const geom::vec2i_t & p, uint32_t rgb);

    void fill(uint32_t rgb);
};

// keycode table
enum {
    e_key_eof = 0x0,
    // 0x00 -> 0x7f is ascii
    e_key_esc = 0x80,
    e_key_up,
    e_key_down,
    e_key_left,
    e_key_right,
    e_key_lctrl,
    e_key_rctrl,
    e_key_lshift,
    e_key_rshift,
    e_key_lalt,
    e_key_ralt,
    e_key_f1,
    e_key_f2,
    e_key_f3,
};

// encapsulates some window input event
struct window_event_t {

    struct mouse_t {

        enum {
            // lmb
            e_lmb_down    = 0x01,
            e_lmb_click   = 0x02,
            e_lmb_release = 0x04,
            // rmb
            e_rmb_down    = 0x08,
            e_rmb_click   = 0x10,
            e_rmb_release = 0x20,
        };

        int32_t          x, y;
        int32_t          flags_;
        static const int type = 0;
    };

    struct key_t {
        bool             down_;
        uint8_t          key_;
        static const int type = 1;
    };

    uint32_t type_;
    union {
        mouse_t * mouse_;
        key_t *   key_;
    };
};

// input and drawing routines can be grouped and stacked as layers
struct window_layer_t {

    const uint32_t z_;
    bool           visible_;

    window_layer_t(uint32_t z)
        : z_(z)
        , visible_(true)
    {
    }

    // bottom to top draw ordering
    virtual void on_draw(struct window_t *, float delta) = 0;

    // top to bottom event ordering, return true for handled event
    virtual bool on_event(struct window_t *, const window_event_t & event) = 0;
};

// handles all game window operations
struct window_t {

    struct {
        int32_t x_, y_;
        uint8_t prev_buttons_;
    } mouse_;

    window_draw_t draw_;

    static window_t & inst()
    {
        static window_t * wnd = new window_t;
        assert(wnd);
        return *wnd;
    }

    static window_draw_t & draw()
    {
        return inst().draw_;
    }

    void add_layer(window_layer_t * layer);

    bool init(uint32_t width, uint32_t height);
    void free();

    bool tick(float delta);

   private:
    void do_mouse();
    void dispatch_event(const window_event_t & event);

    struct detail_t;
    detail_t * detail_;

    window_t();
};
