#pragma once
#include "../../nomad-game/source/game.h"
#include "../../nomad-headers/source/objects.h"
#include "../../nomad-headers/source/events.h"
#include "../../nomad-util/source/fsm.h"

struct obj_villager_t : public fsm_t<obj_villager_t>, object::object_t {

    enum class role_t {
        e_idle,
        e_farmer,
        e_lumberjack,
        e_swordsman,
        e_archer,
        e_wizard
    };

    geom::vec2i_t        dest_;
    object::object_ref_t obj_;
    uuid::player_uuid_t  player_;
    role_t               role_;
    uint32_t             health_;

    // ctor
    obj_villager_t(uint32_t uuid)
        : object_t(uuid, type())
        , fsm_t<obj_villager_t>(*this)
    {
    }

    // state machine states

    void on_frame_walk(nomad_game_t &);
    void on_frame_idle(nomad_game_t &);
    void on_event_root(nomad_game_t &, const event::event_t &);

    static const fsm_state_t<obj_villager_t> fs_walking;
    static const fsm_state_t<obj_villager_t> fs_idle;

    // object_t interface

    virtual void on_spawn(nomad_game_t &) override;
    virtual void on_frame(nomad_game_t &) override;
    virtual void on_event(nomad_game_t &, const event::event_t &) override;

    static object::class_t type()
    {
        return object::class_t::e_villager;
    }
};
