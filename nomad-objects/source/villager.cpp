#include "villager.h"

void obj_villager_t::on_event_root(nomad_game_t &, const event::event_t & e)
{
    switch (e.type()) {
    case (event::e_object_move) :
        dest_.x = e.get<event::object_move_t>().x_;
        dest_.y = e.get<event::object_move_t>().y_;
        set_state(fs_walking, true);
    }
}

void obj_villager_t::on_frame_walk(nomad_game_t & game)
{
    int32_t dx = dest_.x - pos_[1].x;
    int32_t dy = dest_.y - pos_[1].y;

    pos_[0] = pos_[1];
    pos_[1].x += dx/10;
    pos_[1].y += dy/10;

    if (geom::distance_sqr(pos_[1], dest_)<64) {
        set_state(fs_idle, true);
    }
}

void obj_villager_t::on_frame_idle(nomad_game_t & game)
{
    pos_[0] = pos_[1];
}

void obj_villager_t::on_spawn(nomad_game_t & game)
{
    set_state(fs_idle, true);
}

void obj_villager_t::on_frame(nomad_game_t & game)
{
    fsm_t<obj_villager_t>::on_tick(game);
}

void obj_villager_t::on_event(nomad_game_t & game, const event::event_t & event)
{
    fsm_t<obj_villager_t>::on_event(game, event);
}

const fsm_state_t<obj_villager_t> obj_villager_t::fs_walking(
    nullptr,
    &(obj_villager_t::on_frame_walk),
    &(obj_villager_t::on_event_root),
    nullptr,
    nullptr,
    nullptr);

const fsm_state_t<obj_villager_t> obj_villager_t::fs_idle(
    nullptr,
    &(obj_villager_t::on_frame_idle),
    &(obj_villager_t::on_event_root),
    nullptr,
    nullptr,
    nullptr);
