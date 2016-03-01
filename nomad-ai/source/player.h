#pragma once
#include "../../nomad-sdk/source/nomad.h"

// computer player factory function
nomad::player_t *create_player_ai(nomad::game_view_t *view,
                                  nomad::stream_t &stream,
                                  nomad::player_uuid_t uuid);
