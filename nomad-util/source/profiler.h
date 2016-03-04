#pragma once

struct profile_t {

    static void push(void *)
    {
    }
};

#define PROF() profile_t _prof_((void *)__FUNCTION__);
