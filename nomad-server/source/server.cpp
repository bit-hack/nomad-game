#include <memory>
#include <thread>
#include <mutex>
#include <cassert>
#include <vector>
#include <chrono>

#include "server.h"

#include "../../nomad-net/source/net.h"
#include "../../nomad-util/source/sdl.h"
#include "../../nomad-util/source/socket.h"
#include "../../nomad-util/source/log.h"
#include "../../nomad-sdk/source/events.h"

namespace server
{

struct player_t {

    uuid::player_uuid_t uuid_;
    uint8_t              nick_[16];
    bool                 ready_;

    player_t(uuid::player_uuid_t uuid)
        : ready_(false)
        , uuid_(uuid)
    {
    }

    void to_state(event::player_state_t & out) const
    {
        memcpy(out.nick_, nick_, sizeof(nick_));
        out.ready_ = ready_;
        out.uuid_  = uuid_;
    }

    void from_state(const event::player_state_t & in)
    {
        uuid_ = in.uuid_;
        memcpy(nick_, in.nick_, sizeof(nick_));
        ready_ = in.ready_;
    }
};

struct peer_t : public net::client_t {

    // list of all players served by this client
    std::vector<player_t> players_;

    peer_t(net::net_socket_t socket)
        : net::client_t(socket)
    {
    }
};

struct server_t {

    bool                         active_;
    uint32_t                     uuid_;
    uint16_t                     port_;
    std::vector<peer_t>          clients_;
    std::unique_ptr<std::thread> thread_;
    bool                         begun_;

    void reset(uint16_t port)
    {
        active_ = false;
        port_   = port;
        uuid_   = 0;
        begun_  = false;
    }

    server_t()
        : begun_(false)
        , active_(false)
    {
        reset(1234);
    }

    // spin up the server
    bool start(uint16_t port)
    {
        assert(!thread_);
        reset(port);
        active_ = false;
        thread_.reset(new std::thread(trampoline, this));
        while (!active_) {
            std::this_thread::yield();
        }
        return true;
    }

    // kill the server
    void stop()
    {
        assert(thread_);
        active_ = false;

        // kill accept thread
        if (thread_->joinable())
            thread_->join();
        thread_.reset();

        // close all clients
        for (peer_t & client : clients_)
            client.close();

        clients_.clear();
    }

    // create a listen socket
    SOCKET make_listen_sock()
    {
        // setup listen socket address
        sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
#if defined(__linux__)
        addr.sin_addr.s_addr = INADDR_ANY;
#elif defined(WIN32)
        addr.sin_addr.S_un.S_addr = INADDR_ANY;
#endif
        addr.sin_port = htons(port_);

        // start the listen socket
        SOCKET server = socket(AF_INET, SOCK_STREAM, 0);
        if (server == INVALID_SOCKET)
            return INVALID_SOCKET;
        if (bind(server, (sockaddr *)&addr, sizeof(addr)))
            return INVALID_SOCKET;
        if (listen(server, SOMAXCONN) == SOCKET_ERROR)
            return INVALID_SOCKET;

        // return listen socket
        return server;
    }

    // check listen socket for new clients
    bool accept_clients(SOCKET server)
    {
        // check if there is a pending connection
        {
            fd_set set;
            FD_ZERO(&set);
            FD_SET(server, &set);
            timeval time = {0, 0};
            if (select(0, &set, nullptr, nullptr, &time) == 0)
                // well at least we tried
                return true;
        }
        // accept the new connection
        SOCKET sock = accept(server, nullptr, nullptr);
        if (sock == INVALID_SOCKET)
            return false;
        // book keeping for new client
        {
            // create a new client object
            LOG(log_t::e_log_server, "new client");
            peer_t c = peer_t(net::net_socket_t(sock));
            clients_.push_back(c);
            // send snapshot of all players
            for (peer_t & client : clients_) {
                for (player_t & player : client.players_) {
                    event::player_state_t state;
                    player.to_state(state);
                    client.send(state);
                }
            }
        }
        // success
        return true;
    }

    // send a received message to all connected clients
    bool relay(const event::event_t & event)
    {
        for (peer_t & client : clients_)
            if (!client.send(event))
                return false;
        return true;
    }

    // send a new message to all connected clients
    template <typename type_t>
    bool broadcast(const type_t & event)
    {
        for (peer_t & client : clients_)
            if (!client.send(event))
                return false;
        return true;
    }

    // a client has requested a uuid for a new player.  We need to send one
    // back to them.
    void on_request_uuid(peer_t & client)
    {
        LOG(log_t::e_log_server, "");

        // if the game has begun then no new players may be created
        if (begun_)
            return;
        // get a new uuid
        uint16_t uuid = uuid_++;
        {
            // send new uuid packet
            event::receive_uuid_t e;
            e.uuid_ = uuid;
            client.send(e);
            client.players_.push_back(player_t(uuid));
            player_t & player = client.players_.back();
        }
        {
            // tell everyone else this player has joined
            event::player_joined_t e;
            e.uuid_ = uuid;
            for (peer_t & peer : clients_)
                client.send(e);
        }
    }

    // a client has send us a player state.
    void on_player_state(peer_t & client, const event::player_state_t & e)
    {
        LOG(log_t::e_log_server, "");

        // only this client should be responsible for telling us that its
        // players are ready so only need look for players it serves.

        for (player_t & player : client.players_) {
            if (player.uuid_ == e.uuid_) {
                player.from_state(e);
                broadcast(e);
                return;
            }
        }

        LOG(log_t::e_log_server, "unknown player uuid");
    }

    // when a player toggles their ready status we run this function to check
    // if everyone is ready and so the game should begin.
    void try_begin()
    {
        // check if all the players are ready
        for (const peer_t & client : clients_)
            for (const player_t & player : client.players_)
                if (!player.ready_)
                    return;
        {
            begun_ = true;
            event::game_begin_t e;
            auto now = std::chrono::system_clock::now().time_since_epoch();
            e.seed_  = now.count();
            broadcast(e);
        }
    }

    // we have received an event from a client, so we dispatch it based on its
    // type.  some events are server only and others relayed to all clients.
    bool recv_event(peer_t & client, const event::event_t & event)
    {
        // filter out events being sent to the server specifically
        switch (event.header_.type_) {
        case (event::e_request_uuid): on_request_uuid(client); break;

        case (event::e_player_state):
            // if the match has yet to start
            if (!begun_) {
                on_player_state(client, event.get<event::player_state_t>());
                try_begin();
            }
            break;

        default:
            // send this message to every client
            if (!relay(event))
                return false;
        }
        // success
        return true;
    }

    // poll all sockets and read any events that are available.
    bool poll_sockets()
    {
        event::event_t event;

        // read from all sockets
        for (peer_t & client : clients_) {
            // if this client has data waiting
            while (client.pending()) {
                // receive and event from this client
                if (!client.recv(event)) {
                    return false;
                }
                // send on to the event handler
                if (!recv_event(client, event)) {
                    return false;
                }
            }
        }
        return true;
    }

    // thread: accept incoming socket connections
    void thread_func()
    {
        SOCKET server = make_listen_sock();
        LOG(log_t::e_log_server, "server listening");

        active_ = true;
        while (active_ /* thread is alive */) {

            // give time back to the OS
            std::this_thread::yield();

            // check for new connection
            if (!begun_)
                if (!accept_clients(server))
                    return;

            if (!clients_.empty()) {
                fd_set temp;
                FD_ZERO(&temp);
                for (peer_t & client : clients_)
                    FD_SET(client.socket_, &temp);
                timeval time = {0, 1000};
                if (select(0, &temp, nullptr, nullptr, &time)) {
                    poll_sockets();
                }
            }
        }
// close the listen server
#if defined(__linux__)
        close(server);
#elif defined(WIN32)
        closesocket(server);
#endif
    }

    // thread launch trampoline
    static void trampoline(server_t * self)
    {
        assert(self);
        self->thread_func();
    }
};

server_t * server_start(uint16_t port)
{
    std::unique_ptr<server_t> server(new server_t);
    if (!server->start(port))
        return nullptr;
    return server.release();
}

void server_stop(server_t * s)
{
    assert(s);
    s->stop();
}
}  // namespace server
