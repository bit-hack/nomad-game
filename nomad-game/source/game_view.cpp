#include "game.h"
#include "game_view.h"

// ctor
nomad_game_view_t::nomad_game_view_t(
    nomad_game_t * game, uuid::player_uuid_t uuid)
    : ngame_(*game)
    , game::game_view_t(*game, uuid)
{
}

// query for objects in a rectangular region
bool nomad_game_view_t::query_obj_rect_map(
    const geom::rect2i_t & in, std::vector<object::object_ref_t> & out)
{
    return ngame_.db_.query_obj_rect_map(in, out);
}

// query for objects in a given radius
bool nomad_game_view_t::query_obj_radius_map(
    const geom::vec2i_t &               in,
    const int32_t                       radius,
    std::vector<object::object_ref_t> & out)
{
    return ngame_.db_.query_obj_radius_map(in, radius, out);
}

// get map tile information
bool nomad_game_view_t::get_map_info(
    const geom::rect2i_t & in, game::map_info_t & out)
{
    return false;
}

// lookup and object by its uuid
const object::object_ref_t nomad_game_view_t::lookup(uuid::object_uuid_t uuid)
{
    return ngame_.db_.find(uuid);
}
