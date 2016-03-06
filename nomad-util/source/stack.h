#pragma once
#include <stdint.h>
#include <array>

template <typename type_t, size_t size>
struct stack_t {
    
    std::array<type_t, size> item_;
    size_t head_;

    type_t & top()
    {
        assert(head_);
        return item_[head_-1];
    }

    const type_t & top() const
    {
        assert(head_);
        return item_[head_-1];
    }

    void push(const type_t & in)
    {
        assert(head_ < item_.size());
        item_[head_++] = in;
    }

    void pop()
    {
        assert(head_);
        --head_;
    }

    void clear()
    {
        head_ = 0;
    }

    bool empty() const
    {
        return head_ == 0;
    }

    bool ifull() const
    {
        return head_>=item_.size();
    }
};
