#pragma once
#include "../../nomad-headers/source/game.h"
#include "../../nomad-headers/source/player.h"

struct nomad_t;

// create a nomad instance
nomad_t * nomad_init(game::game_t *, const char * address, bool host);

// destroy a nomad instance
void nomad_free(nomad_t *);

// add a player to nomad
void nomad_add_player(nomad_t * nomad, player::player_factory_t factory);

// enter nomads main loop
void nomad_run(nomad_t * nomad);
