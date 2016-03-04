#pragma once

#include <stdio.h>
#include <mutex>
#include <array>
#include <stdarg.h>

struct log_t {

    enum {
        e_log_net    = 0x01,
        e_log_server = 0x02,
        e_log_nomad  = 0x04,
        e_log_window = 0x08,
        e_log_game   = 0x10,
        e_log_player = 0x20,
        e_log_ai     = 0x40,
        e_log_all    = 0xffff
    };

    void redirect(FILE * fd)
    {
        fd_ = fd;
    }

    void printf(const char * fmt, ...)
    {
        if (fd_) {
            std::lock_guard<std::mutex> guard(mux_);
            std::array<char, 256> t;
            va_list ap;
            va_start(ap, fmt);
            vsnprintf(t.data(), t.size(), fmt, ap);
            fputs(t.data(), fd_);
            fputs("\n", fd_);
            va_end(ap);
            if (flushed_) {
                fflush(fd_);
            }
        }
    }

    void enable(uint32_t channel)
    {
        mask_ |= channel;
    }

    void disable(uint32_t channel)
    {
        mask_ &= ~channel;
    }

    bool is_enabled(uint32_t channel) const
    {
        return (mask_ & channel) != 0;
    }

    static log_t & inst()
    {
        static log_t * ptr = new log_t;
        return *ptr;
    }

   protected:
    log_t()
        : fd_(stdout)
        , mux_()
        , mask_(0)
        , flushed_(false)
    {
    }

    ~log_t()
    {
        if (fd_)
            fclose(fd_);
    }

    log_t(const log_t &)  = delete;
    log_t(const log_t &&) = delete;

    FILE *     fd_;       // output file descriptor
    std::mutex mux_;      // write mutex
    uint32_t   mask_;     // enable mask
    bool       flushed_;  // force flush
};

#define LOG(Y, X)                                                              \
    {                                                                          \
        if (log_t::inst().is_enabled(Y))                                       \
            log_t::inst().printf("%s - " X, __FUNCTION__);                     \
    }

#define LOGF(Y, X, ...)                                                        \
    {                                                                          \
        if (log_t::inst().is_enabled(Y))                                       \
            log_t::inst().printf("%s - " X, __FUNCTION__, __VA_ARGS__);        \
    }
