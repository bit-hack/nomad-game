#pragma once
#include <map>
#include <vector>

#include "forward.h"
#include "../../nomad-headers/source/uuid.h"
#include "../../nomad-headers/source/objects.h"
#include "../../nomad-util/source/fsm.h"

// db_t forms the object and actor database
struct db_t {

    // ctor
    db_t(nomad_game_t &);

    void insert(object::object_t * obj);

    object::object_ref_t find(uuid::object_uuid_t id);

    void on_frame();

    // query for objects in a rectangular region
    bool query_obj_rect_map(
        const geom::rect2i_t & in, std::vector<object::object_ref_t> & out);

    // query for objects in a given radius
    bool query_obj_radius_map(
        const geom::vec2i_t &               in,
        const int32_t                       radius,
        std::vector<object::object_ref_t> & out);

   protected:
    std::map<uuid::object_uuid_t, object::object_t *> obj_map_;
    nomad_game_t & game_;
};
