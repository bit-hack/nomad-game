#include <assert.h>

#include "../../nomad-util/source/sdl.h"
#include "../../nomad-sdk/source/nomad.h"
#include "../../nomad/source/nomad.h"
#include "../../nomad-human/source/player.h"
#include "../../nomad-ai/source/player.h"
#include "../../nomad-game/source/game.h"
#include "../../nomad-util/source/log.h"
#include "../../nomad-window/source/window.h"

int main(int argc, const char **args) {
    // init the SDL library
    if (SDL_Init(SDL_INIT_VIDEO)) {
        LOG(log_t::e_log_nomad, "SDL_Init failed");
        return -1;
    }

    if (argc != 3) {
        printf("usage: <A.B.C.D:PORT> <true/false>");
        return -1;
    }

    // check if we are to host
    bool host = (strncmp(args[2], "true", 4) == 0);

    // create the game code instance
    nomad::game_t *game = create_game_nomad();
    if (!game) {
        LOG(log_t::e_log_nomad, "create_game_nomad failed");
        return -1;
    }
    // create an instance of nomad
    nomad_t *nomad = nomad_init(game, args[1], host);
    if (!nomad) {
        LOG(log_t::e_log_nomad, "nomad_init failed")
        return -1;
    }
    // create the game window
    if (!window_t::inst().init(320, 240)) {
        LOG(log_t::e_log_nomad, "window_t::init failed");
        return -1;
    }
    // add a human player
    nomad_add_player(nomad, create_player_human);
    // add an AI player
    nomad_add_player(nomad, create_player_ai);
    // enter the nomad mail loop
    nomad_run(nomad);
    // kill the nomad instance
    nomad_free(nomad);
    // kill the game window
    window_t::inst().free();
    // kill SDL library
    SDL_Quit();

    return 0;
}
