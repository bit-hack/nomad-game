#include "villager.h"

void obj_villager_t::on_frame_walk()
{
}

void obj_villager_t::on_frame_idle()
{
}

void obj_villager_t::on_spawn(nomad_game_t & game)
{
}

void obj_villager_t::on_frame(nomad_game_t & game)
{
    fsm_t<obj_villager_t>::on_tick();
}

void obj_villager_t::on_event(nomad_game_t & game, const event::event_t & event)
{
    fsm_t<obj_villager_t>::on_event();
}

const fsm_state_t<obj_villager_t> obj_villager_t::fs_walking(
    nullptr,
    &(obj_villager_t::on_frame_walk),
    nullptr,
    nullptr,
    nullptr,
    nullptr);

const fsm_state_t<obj_villager_t> obj_villager_t::fs_idle(
    nullptr,
    &(obj_villager_t::on_frame_idle),
    nullptr,
    nullptr,
    nullptr,
    nullptr);
