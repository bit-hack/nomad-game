#include "../../nomad-net/source/net.h"
#include "../../nomad-sdk/source/events.h"
#include "../../nomad-sdk/source/nomad.h"
#include "../../nomad-util/source/log.h"
#include "../../nomad-window/source/window.h"
#include "player.h"

struct player_human_t;

// the human player layer is responsible for receiving user input and rendering
// the game to the output window.
struct player_human_layer_t: public window_layer_t {

    player_human_t * player_;

    // ctor
    player_human_layer_t(player_human_t * player)
        : player_(player)
        , window_layer_t(1)
    {
    }

    // bottom to top draw ordering
    virtual void on_draw(window_t *) override;

    // top to bottom event ordering, return true for handled event
    virtual bool on_event(window_t *, const window_event_t & event) override;
};

// the human player class is responsible for receiving events and queues from
// nomad_t and game_t.
struct player_human_t: public nomad::player_t {

    player_human_layer_t layer_;

    // ctor
    player_human_t(nomad::game_view_t *view,
                   nomad::stream_t &stream,
                   nomad::player_uuid_t uuid)
        : player_t(view, stream, uuid)
        , layer_(this)
    {
        // add the human player IO layer
        window_t::inst().add_layer(&layer_);
    }

    // receipt of an cue event from game_t
    virtual void on_recv(const nomad::cue_t &cue) override;

    // called in a tight loop indepentend from the logic framerate
    virtual void on_tick(float delta) override;

    // called everytime we receive a frame event from the relay
    virtual void on_frame() override;
};

void player_human_layer_t::on_draw(window_t *) {
}

bool player_human_layer_t::on_event(window_t *, const window_event_t & event) {

    if (event.type_==window_event_t::key_t::type()) {

        const uint8_t key = event.key_->key_;

        if (key=='r') {
            // the AI is always good to go
            event::player_state_t e = {
                player_->uuid_,
                true,
                {"human"}
            };
            player_->stream_.send(e, player_->uuid_);
        }

        if (key<=0x7f) {
            LOGF(log_t::e_log_player, "key event: %c", key);
        }
        else {
            LOGF(log_t::e_log_player, "key event: %hhu", key);
        }
    }

    return false;
}

void player_human_t::on_recv(const nomad::cue_t &cue) {
}

void player_human_t::on_tick(float delta) {
}

void player_human_t::on_frame() {
}

// human player factory function
nomad::player_t *create_player_human(nomad::game_view_t *view,
                                     nomad::stream_t &stream,
                                     nomad::player_uuid_t uuid) {
    return new player_human_t(view, stream, uuid);
}
