#pragma once

#include <array>
#include <stdint.h>

template <typename type_t, size_t c_size>
struct small_queue_t {
    small_queue_t()
        : head_(0)
        , tail_(0)
    {
    }

    void push(const type_t & in)
    {
        ring_[(head_++) % c_size] = in;
    }

    type_t pop()
    {
        return ring_[(tail_++) % c_size];
    }

   protected:
    std::array<type_t, c_size> ring_;
    uint32_t head_, tail_;
};
