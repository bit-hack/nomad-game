#include "factory.h"
#include "db.h"
#include "game.h"

#include "../../nomad-objects/source/villager.h"

factory_t::factory_t(nomad_game_t & game)
    : uuid_(0)
    , game_(game)
{
}

object::object_t * factory_t::create_object(
    object::class_t type, const geom::vec2i_t & pos)
{
    object::object_t * obj = nullptr;

    switch (type) {
    case (object::class_t::e_villager):
        obj = new obj_villager_t(++uuid_);
        {
            obj_villager_t & villager = obj->cast<obj_villager_t>();
            villager.player_          = 0;
            villager.role_            = obj_villager_t::role_t::e_idle;
            villager.health_          = 100;
        }
        break;

    default: assert(!"unknown object type"); return obj;
    }

    obj->pos_[0] = pos;
    obj->pos_[1] = pos;

    game_.db_.insert(obj);
    obj->on_spawn(game_);
    return obj;
}
