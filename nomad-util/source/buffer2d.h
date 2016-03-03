#pragma once
#include <stdint.h>

template <typename type_t>
struct buffer2d_t {

    buffer2d_t(size_t w, size_t h)
        : data_(new type_t[ w * h ])
    {
    }

    buffer2d_t(buffer2d_t & copy) {
        
    }
    
    const size_t width, height;
    std::unique_ptr<type_t> data_;

    const type_t & get(int32_t x, int32_t y) const {
        return data_[ x + y * width ];
    }
    
    type_t & get(int32_t x, int32_t y) {
        return data_[ x + y * width ];
    }
};
