#pragma once
#include <stdint.h>

#include "uuid.h"

namespace event
{
enum type_t {

    e_none,

    // client to server

    e_request_uuid,  // request/recive player uuid
    e_ping,          // ping packet, relay server sends pack ping
    e_player_state,  // a players state has changed

    // server to client

    e_game_begin,     // the game must start
    e_recive_uuid,    // server gives uuid to client
    e_player_joined,  // a new player has joined
    e_player_leave,   // a player has left

    // client to client

    e_game_pause,  // pause/resume the game (toggles)
    e_game_frame,  // advance game_t one tick
};

// every game event has a header which encapsulates and abstracts the different
// possible event bodies.
struct event_header_t {
    uint16_t size_;
    uint16_t type_;
    uint16_t checksum_;
};

// abstract representation for a game event.
struct event_t {
    static const size_t c_max_size = 128;
    typedef std::array<uint8_t, c_max_size> event_body_t;

    event_header_t header_;
    event_body_t   body_;

    // down cast an events body for reading
    template <typename type_t>
    const type_t & get() const
    {
        assert(header_.size_ == sizeof(type_t));
        assert(header_.type_ == type_t::type());
        return *reinterpret_cast<const type_t *>(body_.data());
    }
};

struct receive_uuid_t {
    uint16_t      uuid_;
    static type_t type()
    {
        return e_recive_uuid;
    }
};

struct ping_t {
    uint32_t      data_;
    static type_t type()
    {
        return e_ping;
    }
};

struct game_frame_t {
    uint32_t      data_;
    static type_t type()
    {
        return e_game_frame;
    }
};

struct game_begin_t {
    uint64_t      seed_;
    static type_t type()
    {
        return e_game_begin;
    }
};

struct player_joined_t {
    uuid::player_uuid_t uuid_;
    static type_t       type()
    {
        return e_player_joined;
    }
};

// when the player state changes this gets broadcast
struct player_state_t {
    uuid::player_uuid_t uuid_;
    bool                ready_;
    char                nick_[16];
    static type_t       type()
    {
        return e_player_state;
    }
};

}  // namespace event
