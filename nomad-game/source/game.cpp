#include "game.h"
#include "../../nomad-sdk/source/game.h"
#include "../../nomad-sdk/source/events.h"
#include "../../nomad-sdk/source/objects.h"
#include "../../nomad-util/source/log.h"

struct controller_t {
    object::object_t & self;

    void think()
    {
    }
};

struct nomad_game_t : public game::game_t {
    std::vector<object::object_t *> objects_;

    // ctor
    nomad_game_t()
        : objects_()
    {
    }

    // called for each frame packet received
    void on_frame(const event::game_frame_t & e)
    {
    }

    void on_begin(const event::game_begin_t & e)
    {
        LOGF(log_t::e_log_game, "game begin { seed=%u }", e.seed_);
    }

    virtual void recv(const event::event_t & e) override
    {
        switch (e.header_.type_) {
        case (event::e_game_frame):
            on_frame(e.get<event::game_frame_t>());
            break;
        case (event::e_game_begin):
            on_begin(e.get<event::game_begin_t>());
            break;
        }
    }

    virtual game::game_view_t * get_view(uuid::player_uuid_t uuid) override;
};

struct nomad_game_view_t : public game::game_view_t {

    nomad_game_t & ngame_;

    // ctor
    nomad_game_view_t(nomad_game_t * game, uuid::player_uuid_t uuid)
        : ngame_(*game)
        , game::game_view_t(*game, uuid)
    {
    }

    // query for objects in a rectangular region
    virtual bool query_obj_rect_map(
        const geom::rect2i_t &                 in,
        std::vector<const object::object_t *> & out) override
    {
        for (object::object_t * obj : ngame_.objects_) {
            assert(obj);
            if (geom::inside(in, obj->pos_[1])) {
                out.push_back(obj);
            }
        }
        return true;
    }

    // query for objects in a given radius
    virtual bool query_obj_radius_map(
        const geom::rect2i_t & in, const float radius,
        std::vector<const object::object_t *> & out) override
    {
        return false;
    }

    // get map tile information
    virtual bool get_map_info(
        const geom::rect2i_t & in, game::map_info_t & out) override
    {
        return false;
    }

    // lookup and object by its uuid
    virtual const object::object_t * lookup(uuid::object_uuid_t uuid) override
    {
        for (object::object_t * obj : ngame_.objects_) {
            assert(obj);
            if (obj->uuid_ == uuid)
                return obj;
        }
        return nullptr;
    }
};

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
