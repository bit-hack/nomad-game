#pragma once
#include <vector>
#include "../../nomad-util/source/geometric.h"
#include "../../nomad-headers/source/objects.h"

#include "forward.h"

struct nomad_game_view_t : public game::game_view_t {

    nomad_game_t & ngame_;

    // ctor
    nomad_game_view_t(nomad_game_t * game, uuid::player_uuid_t uuid);

    // query for objects in a rectangular region
    virtual bool query_obj_rect_map(
        const geom::rect2i_t &              in,
        std::vector<object::object_ref_t> & out) override;

    // query for objects in a given radius
    virtual bool query_obj_radius_map(
        const geom::vec2i_t &               in,
        const int32_t                       radius,
        std::vector<object::object_ref_t> & out) override;

    // get map tile information
    virtual bool get_map_info(
        const geom::rect2i_t & in, game::map_info_t & out) override;

    // lookup and object by its uuid
    virtual const object::object_ref_t lookup(
        uuid::object_uuid_t uuid) override;
};
