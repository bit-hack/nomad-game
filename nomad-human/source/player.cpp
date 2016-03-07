#include "../../nomad-net/source/net.h"
#include "../../nomad-headers/source/events.h"
#include "../../nomad-headers/source/objects.h"
#include "../../nomad-headers/source/game.h"
#include "../../nomad-util/source/log.h"
#include "../../nomad-window/source/window.h"

#include "../../nomad-objects/source/villager.h"

#include "player.h"

struct player_human_t;

// the human player layer is responsible for receiving user input and rendering
// the game to the output window.
struct player_human_layer_t : public window_layer_t {

    player_human_t *    player_;
    uuid::object_uuid_t selected_;

    // ctor
    player_human_layer_t(player_human_t * player)
        : player_(player)
        , window_layer_t(1)
        , selected_(-1)
    {
    }

    void on_key_event(const window_event_t & event);

    void on_mouse_event(const window_event_t & event);

    // bottom to top draw ordering
    virtual void on_draw(window_t *, float delta) override;

    // top to bottom event ordering, return true for handled event
    virtual bool on_event(window_t *, const window_event_t & event) override;
};

// the human player class is responsible for receiving events and queues from
// nomad_t and game_t.
struct player_human_t : public player::player_t {

    player_human_layer_t layer_;

    struct {
        std::array<char, 16> name_;
        bool ready_;
    } info_;

    // ctor
    player_human_t(
        game::game_view_t * view,
        player::stream_t &  stream,
        uuid::player_uuid_t uuid)
        : player_t(view, stream, uuid)
        , layer_(this)
    {
        // setup player state
        info_.ready_ = false;
        memcpy(info_.name_.data(), "human", 6);
        send_player_state();
        // add the human player IO layer
        window_t::inst().add_layer(&layer_);
    }

    //
    void send_player_state()
    {
        event::player_state_t e;
        e.uuid_  = uuid_;
        e.ready_ = info_.ready_;
        memcpy(e.nick_, info_.name_.data(), info_.name_.size());
        stream_.send(e, uuid_);
    }

    // receipt of an cue event from game_t
    virtual void on_recv(const game::cue_t & cue) override;

    // called in a tight loop indepentend from the logic framerate
    virtual void on_tick(float delta) override;

    // called everytime we receive a frame event from the relay
    virtual void on_frame() override;
};

void player_human_layer_t::on_draw(window_t * wnd, float delta)
{
    std::vector<object::object_ref_t> found_;
    {
        geom::rect2i_t area{0, 0, 512, 512};
        player_->view_->query_obj_rect_map(area, found_);
    }
    for (const object::object_ref_t & obj : found_) {

        uint32_t rgb = 0x113399;

        if (obj->type_ == object::class_t::e_villager) {
            obj_villager_t & v = obj->cast<obj_villager_t>();
            if (v.player_ == player_->uuid_) {
                rgb = 0x4466CC;
            }
            if (v.uuid_ == selected_) {
                rgb = 0xdddddd;
            }
        }

        int32_t ix((obj->pos_[0].x * (1 - delta)) + (obj->pos_[1].x * delta));
        int32_t iy((obj->pos_[0].y * (1 - delta)) + (obj->pos_[1].y * delta));

        window_t::draw().circle(geom::vec2i_t{ix, iy}, 4, rgb);
    }
}

void player_human_layer_t::on_key_event(const window_event_t & event)
{
    const uint8_t key = event.key_->key_;

    if (key == e_key_f1 && event.key_->down_) {
        LOGF(
            log_t::e_log_player,
            "set player:%d ready state:%d",
            player_->uuid_,
            player_->info_.ready_);
        player_->info_.ready_ ^= true;
        player_->send_player_state();
    }

    if (key <= 0x7f) {
        LOGF(log_t::e_log_player, "key event: %c", key);
    } else {
        LOGF(log_t::e_log_player, "key event: %hhu", key);
    }
}

void player_human_layer_t::on_mouse_event(const window_event_t & event)
{
    window_event_t::mouse_t & mouse = *event.mouse_;

    if (mouse.flags_ & mouse.e_lmb_click) {
        // find nearest object
        geom::vec2i_t                     pos{mouse.x, mouse.y};
        std::vector<object::object_ref_t> found_;
        player_->view_->query_obj_radius_map(pos, 4, found_);
        if (found_.size()) {

            if (found_[0]->cast<obj_villager_t>().player_ == player_->uuid_) {
                selected_ = found_[0]->uuid_;
            }
        } else {
            selected_ = -1;
        }
    }

    if (mouse.flags_ & mouse.e_rmb_click) {
        if (selected_ != -1) {
            // move to location
            event::object_move_t move;
            move.uuid_ = selected_;
            move.x_    = mouse.x;
            move.y_    = mouse.y;
            player_->stream_.send(move, player_->uuid_);
        }
    }
}

bool player_human_layer_t::on_event(window_t *, const window_event_t & event)
{
    switch (event.type_) {
    case (window_event_t::key_t::type): on_key_event(event); break;
    case (window_event_t::mouse_t::type): on_mouse_event(event); break;
    }
    return true;
}

void player_human_t::on_recv(const game::cue_t & cue)
{
}

void player_human_t::on_tick(float delta)
{
}

void player_human_t::on_frame()
{
}

// human player factory function
player::player_t * create_player_human(
    game::game_view_t * view,
    player::stream_t &  stream,
    uuid::player_uuid_t uuid)
{
    return new player_human_t(view, stream, uuid);
}
