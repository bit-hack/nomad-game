#pragma once

#include <stdio.h>
#include <mutex>
#include <array>
#include <stdarg.h>

struct log_t {

    void printf(const char *fmt, ...) {
        std::lock_guard<std::mutex> guard(mux_);
        std::array<char, 256> t;
        va_list ap;
        va_start(ap, fmt);
        vsnprintf(t.data(), t.size(), fmt, ap);
        fputs(t.data(), fd_);
        fputs("\n", fd_);
        va_end(ap);
    }

    static log_t &inst() {
        static log_t *ptr = new log_t;
        return *ptr;
    }

  protected:
    log_t() : fd_(stdout), mux_() {}

    FILE *fd_;
    std::mutex mux_;
};


#define LOG(X)                                                                 \
    { log_t::inst().printf("%s - " X, __FUNCTION__); }

#define LOGF(X, ...)                                                           \
    { log_t::inst().printf("%s - " X, __FUNCTION__, __VA_ARGS__); }
