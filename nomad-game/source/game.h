#pragma once
#include "../../nomad-headers/source/game.h"

#include "forward.h"
#include "db.h"
#include "factory.h"

#include "../../nomad-headers/source/game.h"
#include "../../nomad-headers/source/events.h"
#include "../../nomad-headers/source/objects.h"



struct nomad_game_t: public game::game_t {
    // object data base
    db_t db_;
    // object factory
    factory_t factory_;
    // random seed
    uint64_t seed_;

    // ctor
    nomad_game_t();

    // called for each frame packet received
    void on_frame(const event::game_frame_t & e);

    void on_begin(const event::game_begin_t & e);

    virtual void recv(const event::event_t & e) override;

    virtual game::game_view_t * get_view(uuid::player_uuid_t uuid) override;
};

game::game_t * create_game_nomad();
