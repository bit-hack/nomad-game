#include "../../nomad-net/source/net.h"
#include "../../nomad-sdk/source/events.h"
#include "../../nomad-sdk/source/nomad.h"
#include "../../nomad-util/source/log.h"
#include "player.h"

struct player_ai_t : nomad::player_t {

    player_ai_t(
        nomad::game_view_t * view, nomad::stream_t & stream,
        nomad::player_uuid_t uuid)
        : player_t(view, stream, uuid)
    {
        // the AI is always good to go
        event::player_state_t e = {uuid, true, {"percy"}};
        stream.send(e, uuid);
    }

    virtual void on_recv(const nomad::cue_t & cue) override
    {
    }

    virtual void on_tick(float delta) override
    {
    }

    virtual void on_frame() override
    { /*LOG("");*/
    }
};

nomad::player_t * create_player_ai(
    nomad::game_view_t * view, nomad::stream_t & stream,
    nomad::player_uuid_t uuid)
{
    return new player_ai_t(view, stream, uuid);
}
