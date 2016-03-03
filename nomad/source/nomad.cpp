#include <SDL.h>
#include <vector>
#include <memory>

#include "nomad.h"

#include "../../nomad-net/source/net.h"
#include "../../nomad-sdk/source/events.h"
#include "../../nomad-sdk/source/nomad.h"
#include "../../nomad-util/source/log.h"
#include "../../nomad-util/source/queue.h"
#include "../../nomad-util/source/timer.h"
#include "../../nomad-window/source/window.h"
#include "../../nomad-server/source/server.h"

// sdl timer
typedef nm_timer_t<uint32_t, SDL_GetTicks> sdl_timer_t;

// the stream object passed to local players to direct events into the global
// event client.
struct nomad_stream_t : public nomad::stream_t {
    struct nomad_t &nomad_;
    bool error_;

    nomad_stream_t(nomad_t &nomad) : nomad_(nomad), error_(false) {}

    bool valid() const { return !error_; }

    virtual void send(const nomad::event_t &e) override;

    virtual void send(const void *src, const size_t size, const uint16_t type,
                      const nomad::player_uuid_t uuid) override;
};

struct nomad_t {
    // the game module
    nomad::game_t *game_;
    // the event client connected to a event relay
    net::client_t client_;
    // the event relay if we are hosting
    server::server_t *server_;
    // list of all local players connected to our game_t
    // todo: replace with std::map<nomad_uuid_t, nomad::player_t*>
    std::vector<nomad::player_t *> players_; 
    // write only event stream from player_t to relay
    std::unique_ptr<nomad::stream_t> stream_;
    // true while the game is running
    bool active_;
    // players that have been added but are waiting for a uuid
    small_queue_t<nomad::player_factory_t, 8> pending_;
    // the frame timer
    sdl_timer_t timer_;

    // ctor
    nomad_t(nomad::game_t *game)
        : game_(game), client_(), server_(nullptr), players_(),
          stream_(new nomad_stream_t(*this)), active_(true), timer_(1000) {}

    // when there is a frame event received
    void on_frame() {
        LOG(log_t::e_log_nomad, "frame");

        // run frame callback for all players
        for (nomad::player_t *player : players_) {
            player->on_frame();
        }

        // if we are not the host then we should reset the timer to more
        // accurately reflect the current expected arrival time of the next
        // frame event.
        if (!server_) {
            timer_.reset();
        }
    }

    // we have received a new uuid for a pending new player
    void on_receive_uuid(const event::receive_uuid_t &e) {
        LOGF(log_t::e_log_nomad, "%d", e.uuid_);

        nomad::player_factory_t factory = pending_.pop();
        assert(factory);
        nomad::game_view_t *view = game_->get_view(e.uuid_);
        assert(view);
        nomad::player_t *player = factory(view, *stream_.get(), e.uuid_);
        assert(player);
        players_.push_back(player);
    }

    //
    void on_player_state(const event::player_state_t &e) {
        LOGF(log_t::e_log_nomad, "%d - %16s", e.uuid_, e.nick_);
    }

    // when we receive a packet from the event relay
    void on_packet(const nomad::event_t &e) {
        LOGF(log_t::e_log_nomad, "%d", e.header_.type_);

        // other modules may like to know about frames
        switch (e.header_.type_) {
        case (event::type_t::e_game_frame):
            on_frame();
            break;

        case (event::e_recive_uuid):
            // complete pending new player request with this uuid
            on_receive_uuid(e.get<event::receive_uuid_t>());
            return;

        case (event::e_player_state):
            on_player_state(e.get<event::player_state_t>());
            return;
        }

        // pass all packets to the game module
        if (game_) {
            game_->recv(e);
        }
    }

    // send a frame packet to the event relay
    void send_frame() {
        event::game_frame_t frame;
        frame.data_ = 0;
        stream_->send(frame, 0);
    }

    // main game loop
    void run() {
        // while the main game loop is active
        while (active_) {

            // if we are receiving packets
            if (client_.is_ok()) {
                nomad::event_t packet;
                while (client_.pending() && client_.recv(packet)) {
                    on_packet(packet);
                }
            }

            // send periodic frame packets to the relay
            if (server_) {
                while (timer_.delta() > 1.f) {
                    timer_.advance();
                    send_frame();
                }
            }

            // give a time slice to each of the players
            {
                float delta = timer_.delta();
                for (nomad::player_t *player : players_) {
                    assert(player);
                    player->on_tick(delta);
                }
            }

            window_t::draw().fill(0x101010);
            window_t::draw().circle(point_t{64, 64}, 8, 0x909090);

            // tick the main game window
            if (!window_t::inst().tick()) {
                active_ = false;
            }
        }
    }

    // add a player to the current game
    void add_player(nomad::player_factory_t factory) {
        LOG(log_t::e_log_nomad, "");
        pending_.push(factory);
        stream_->send(nullptr, 0, event::e_request_uuid, 0);
    }

    bool connect(const char *address, bool host) {
        LOG(log_t::e_log_nomad, "");

        if (!net::net_init()) {
            LOG(log_t::e_log_nomad, "unable to init netcode");
            return false;
        }
        // start the server
        if (host) {
            LOG(log_t::e_log_nomad, "starting server");
            server_ = server::server_start(1234);
            if (!server_) {
                LOG(log_t::e_log_nomad, "unable to start relay");
                return false;
            }
            // todo: wait for server start (blocking)
        }
        // connect a client to the game relay
        if (!client_.connect(address)) {
            LOG(log_t::e_log_nomad, "unable to connect to relay");
            return false;
        }
        // success
        return true;
    }
};

void nomad_stream_t::send(const nomad::event_t &e) {
    nomad_.client_.send(e);
}

void nomad_stream_t::send(const void *src, const size_t size,
                          const uint16_t type,
                          const nomad::player_uuid_t uuid) {
    nomad_.client_.send(src, size, type); // fixme: uuid
}

nomad_t *nomad_init(nomad::game_t *game, const char *address, bool host) {
    assert(game);
    std::unique_ptr<nomad_t> nomad(new nomad_t(game));
    assert(nomad);
    if (!nomad->connect(address, host)) {
        return nullptr;
    }
    return nomad.release();
}

void nomad_free(nomad_t *game) {
    assert(game);
    delete game;
}

void nomad_add_player(nomad_t *nomad, nomad::player_factory_t factory) {
    assert(nomad);
    nomad->add_player(factory);
}

void nomad_run(nomad_t *nomad) {
    assert(nomad);
    nomad->run();
}
