#pragma once
#include <assert.h>
#include "stack.h"

template <typename type_t>
struct fsm_state_t {

    typedef void (type_t::*on_begin_t)(void);
    typedef void (type_t::*on_end_t)(void);
    typedef void (type_t::*on_yield_t)(void);
    typedef void (type_t::*on_resume_t)(void);

    typedef void (type_t::*on_tick_t)(void);
    typedef void (type_t::*on_event_t)(void);

    fsm_state_t(
        on_begin_t  begin,
        on_tick_t   tick,
        on_event_t  event,
        on_end_t    end,
        on_yield_t  yield,
        on_resume_t resume)
        : on_begin_(begin)
        , on_tick_(tick)
        , on_event_(event)
        , on_end_(end)
        , on_yield_(yield)
        , on_resume_(resume)
    {
    }

    void on_begin(type_t & obj) const
    {
        if (on_begin_)
            (obj.*(on_begin_))();
    }

    void on_yield(type_t & obj) const
    {
        if (on_yield_)
            (obj.*(on_yield_))();
    }

    void on_resume(type_t & obj) const
    {
        if (on_resume_)
            (obj.*(on_resume_))();
    }

    void on_end(type_t & obj) const
    {
        if (on_end_)
            (obj.*(on_end_))();
    }

    void on_tick(type_t & obj) const
    {
        if (on_tick_)
            (obj.*(on_tick_))();
    }

    void on_event(type_t & obj) const
    {
        if (on_event_)
            (obj.*(on_event_))();
    }

   protected:
    // transition events
    on_begin_t  on_begin_;
    on_yield_t  on_yield_;
    on_resume_t on_resume_;
    on_end_t    on_end_;

    // state events
    on_tick_t  on_tick_;
    on_event_t on_event_;
};

template <typename type_t>
struct fsm_t {

    typedef fsm_state_t<type_t> state_t;

    fsm_t(type_t & self)
        : self_(self)
        , state_()
    {
    }

    void on_tick()
    {
        if (!state_.empty()) {
            state_.top()->on_tick(self_);
        }
    }

    void on_event()
    {
        if (!state_.empty()) {
            state_.top()->on_event(self_);
        }
    }

    void set_state(const state_t & new_state, bool retrigger)
    {
        if (!state_.empty()) {
            const state_t *& state = state_.top();
            if (state != &new_state || retrigger) {
                on_end();
                state_.pop();
                state_.push(&new_state);
                on_begin();
            }
        } else {
            state_.push(&new_state);
            on_begin();
        }
    }

    void push_state(const state_t & state)
    {
        if (!state_.empty()) {
            on_yield();
        }
        state_.push(&state);
        on_begin();
    }

    void pop_state()
    {
        if (!state_.empty()) {
            on_end();
            state_.pop();
        }
        if (!state_.empty()) {
            on_resume();
        }
    }

    void pop_all()
    {
        while (!state_.empty()) {
            on_end();
            state_.pop();
        }
    }

   protected:
    void on_begin()
    {
        if (!state_.empty())
            state_.top()->on_begin(self_);
    }

    void on_end()
    {
        if (!state_.empty())
            state_.top()->on_end(self_);
    }

    void on_yield()
    {
        if (!state_.empty())
            state_.top()->on_yield(self_);
    }

    void on_resume()
    {
        if (!state_.empty())
            state_.top()->on_resume(self_);
    }

    // self object instance
    type_t & self_;
    // state stack
    stack_t<const state_t *, 8> state_;
};