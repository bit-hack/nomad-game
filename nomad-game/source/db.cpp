#include "db.h"
#include "factory.h"
#include "game.h"

db_t::db_t(nomad_game_t & game)
    : game_(game)
{
}

void db_t::insert(object::object_t * obj)
{
    assert(obj);
    // insert into object map
    {
        if (find(obj->uuid_)) {
            assert(!"object already exists");
        }
        uuid::object_uuid_t id = obj->uuid_;
        obj_map_[id]           = obj;
    }
}

object::object_ref_t db_t::find(uuid::object_uuid_t id)
{
    auto i = obj_map_.find(id);
    if (i == obj_map_.end()) {
        return object::object_ref_t();
    } else {
        return object::object_ref_t(i->second);
    }
}

void db_t::on_frame()
{
    typedef std::pair<const uuid::object_uuid_t, object::object_t *> pair_t;

    for (auto it = obj_map_.begin(); it!=obj_map_.end(); ++it) {
        object::object_t * obj = it->second;
        assert(obj);
        if (!obj->alive()) {
            delete obj;
            it = obj_map_.erase(it);
        }
        else {
            obj->on_frame(game_);
        }
    }
}

// query for objects in a rectangular region
bool db_t::query_obj_rect_map(
    const geom::rect2i_t & in, std::vector<object::object_ref_t> & out)
{
    typedef std::pair<const uuid::object_uuid_t, object::object_t *> pair_t;

    for (pair_t pair : obj_map_) {

        object::object_t * obj = pair.second;
        assert(obj);
        if (obj->ref_==0) {
            continue;
        }
        else {
            if (geom::inside(in, pair.second->pos_[1]))
                out.push_back(pair.second);
        }
    }
    return true;
}

// query for objects in a given radius
bool db_t::query_obj_radius_map(
    const geom::vec2i_t &               in,
    const int32_t                       radius,
    std::vector<object::object_ref_t> & out)
{
    typedef std::pair<uuid::object_uuid_t, object::object_ref_t> pair_t;
    for (auto pair : obj_map_) {
        auto pos = pair.second->pos_[1];
    }
    return true;
}
