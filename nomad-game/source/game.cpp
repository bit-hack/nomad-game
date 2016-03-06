#include "game.h"
#include "db.h"
#include "factory.h"
#include "game_view.h"

#include "../../nomad-headers/source/game.h"
#include "../../nomad-headers/source/events.h"
#include "../../nomad-headers/source/objects.h"
#include "../../nomad-util/source/log.h"
#include "../../nomad-util/source/numeric.h"
#include "../../nomad-objects/source/villager.h"

// ctor
nomad_game_t::nomad_game_t()
    : db_(*this)
    , factory_(*this)
{
}

// called for each frame packet received
void nomad_game_t::on_frame(const event::game_frame_t & e)
{
    // tick all actors in the database
    db_.on_frame();
}

void nomad_game_t::on_begin(const event::game_begin_t & e)
{
    LOGF(log_t::e_log_game, "game begin { seed=%u }", e.seed_);
    seed_ = e.seed_;

    for (uint32_t i = 0; i < 16; ++i) {

        geom::vec2i_t pos = {numeric::rand64(seed_) % 320,
                             numeric::rand64(seed_) % 240};

        object::object_ref_t obj =
            factory_.create_object(object::class_t::e_villager, pos);

        obj_villager_t & v = obj->cast<obj_villager_t>();
        v.player_          = i / 4;
    }
}

void nomad_game_t::recv(const event::event_t & e)
{
    switch (e.header_.type_) {
    case (event::e_game_frame): on_frame(e.get<event::game_frame_t>()); break;
    case (event::e_game_begin): on_begin(e.get<event::game_begin_t>()); break;
    }
}

// nomad game factory function
game::game_t * create_game_nomad()
{
    return new nomad_game_t;
}

// nomad game view factory function
game::game_view_t * nomad_game_t::get_view(uuid::player_uuid_t uuid)
{
    return new nomad_game_view_t(this, uuid);
}
