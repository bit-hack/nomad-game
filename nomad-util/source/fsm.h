#pragma once
#include "stack.h"

struct fsm_t {

    struct fsm_state_t
    {
        const void (*on_enter_) (fsm_t &);
        const void (*on_frame_) (fsm_t &);
        const void (*on_leave_) (fsm_t &);
        const void (*on_yield_) (fsm_t &);
        const void (*on_resume_)(fsm_t &);
    };

    void push_state(const state_t & s)
    {
        if (!state_.is_empty()) {
            if (state_->on_yield_)
                state_->on_yield_(this);
        }
        state_.push(&s);
        if (state_->on_enter_)
            state_->on_enter_(this);
    }

    void pop_state()
    {
        assert(!state_.is_empty());
        if (state_->on_leave_)
            state_->on_leave_(this);
        state_.pop();
        if (!state_.is_empty()) {
            if (state_->on_resume_)
                state_->on_resume_(this);
        }
    }

    void set_state(const state_t & s)
    {
        if (!state_->is_empty()) {
            if (state_->on_leave_)
                state_->on_leave_(this);
            state_.pop();
        }
        state_.push(s);
        if (state_->on_enter_)
            state_->on_enter_(this);
    }

    void frame() {
        if (!state_->is_empty())
            state_->on_frame_(this);
    }
    
    object_ref_t ref_;

protected:
    stack_t<const fsm_t*, 16> state_;
};
