#pragma once
#include "../../nomad-sdk/source/game.h"
#include "../../nomad-sdk/source/player.h"

// human player factory function
player::player_t * create_player_human(
    game::game_view_t * view,
    player::stream_t & stream,
    uuid::player_uuid_t uuid);
