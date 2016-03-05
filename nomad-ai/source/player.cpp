#include "../../nomad-net/source/net.h"
#include "../../nomad-sdk/source/events.h"
#include "../../nomad-sdk/source/game.h"
#include "../../nomad-util/source/log.h"
#include "player.h"

struct player_ai_t : player::player_t {

    player_ai_t(
        game::game_view_t * view,
        player::stream_t & stream,
        uuid::player_uuid_t uuid)
        : player_t(view, stream, uuid)
    {
        // the AI is always good to go
        event::player_state_t e = {uuid, true, {"percy"}};
        stream.send(e, uuid);
    }

    virtual void on_recv(const game::cue_t & cue) override
    {
    }

    virtual void on_tick(float delta) override
    {
    }

    virtual void on_frame() override
    { /*LOG("");*/
    }
};

player::player_t * create_player_ai(
    game::game_view_t * view,
    player::stream_t & stream,
    uuid::player_uuid_t uuid)
{
    return new player_ai_t(view, stream, uuid);
}
