#pragma once
#include "forward.h"
#include "../../nomad-headers/source/objects.h"

struct factory_t {

    // ctor
    factory_t(nomad_game_t &);

    // create an object
    object::object_t * create_object(
        object::class_t, const geom::vec2i_t & pos);

   protected:
    uint32_t       uuid_;
    nomad_game_t & game_;
};
