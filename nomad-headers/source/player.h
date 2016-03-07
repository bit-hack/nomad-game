#pragma once

#include "uuid.h"
#include "forward.h"

namespace player
{
// stream_t is used by player_t to send events back to the event relay.
struct stream_t {
    // dtor
    virtual ~stream_t()
    {
    }

    // send an event to the event relay
    virtual void send(const event::event_t & event) = 0;

    // send event with arbitary body
    virtual void send(
        const void * src,
        const size_t size,
        const uint16_t type,
        const uuid::player_uuid_t uuid) = 0;

    // send event helper
    template <typename type_t>
    void send(const type_t & t, uuid::player_uuid_t uuid)
    {
        send((const void *)&t, sizeof(type_t), type_t::type, uuid);
    }
};

// player_t abstracts a local player
struct player_t {
    // ctor
    player_t(
        game::game_view_t * view,
        stream_t & stream,
        uuid::player_uuid_t uuid)
        : view_(view)
        , stream_(stream)
        , uuid_(uuid)
    {
    }

    // dtor
    virtual ~player_t()
    {
    }

    // called when game_t generated an audio visual cue
    virtual void on_recv(const game::cue_t & cue) = 0;

    // called as often as possible
    virtual void on_tick(float delta) = 0;

    // called each time the game state is advances
    virtual void on_frame() = 0;

    // used for inspecting the current game state
    game::game_view_t * view_;

    // used to send events to the event relay
    stream_t & stream_;

    // this players unique identifier
    const uuid::player_uuid_t uuid_;
};

// player factory function
typedef player_t * (*player_factory_t)(
    game::game_view_t * view,
    stream_t & stream,
    uuid::player_uuid_t uuid);

}  // namespace nomad
