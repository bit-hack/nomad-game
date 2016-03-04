#pragma once

template <typename type_t, type_t (*func)()>
struct nm_timer_t {
    nm_timer_t(type_t interval)
        : last_(func())
        , interval_(interval)
    {
        assert(interval_ > 0.f);
    }

    float delta()
    {
        assert(interval_ > 0.f);
        type_t dt = func() - last_;
        return float(dt) / float(interval_);
    }

    void advance()
    {
        last_ += interval_;
    }

    void reset()
    {
        last_ = func();
    }

    type_t last_;
    type_t interval_;
};
