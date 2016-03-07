#pragma once
#include <assert.h>

#include "uuid.h"
#include "forward.h"
#include "../../nomad-util/source/ref.h"
#include "../../nomad-util/source/geometric.h"

struct nomad_game_t;

namespace object
{
struct object_t;

enum class class_t {
    e_unknown,
    e_villager,
};

struct object_t {

    object_t(uint32_t uuid, class_t type)
        : uuid_(uuid)
        , type_(type)
        , ref_(1)
    {
    }

    object_t()                 = delete;
    object_t(const object_t *) = delete;

    ~object_t(){};

    const uint32_t uuid_;    // object uuid
    const class_t  type_;    // object type
    uint32_t       ref_;     // reference count
    geom::vec2i_t  pos_[2];  // object position

    //
    bool alive() const {
        return ref_>0;
    }

    // checked downcast
    template <typename type_t>
    type_t & cast()
    {
        assert(type_ == type_t::type());
        return *reinterpret_cast<type_t *>(this);
    }

    // checked downcast
    template <typename type_t>
    const type_t & cast() const
    {
        assert(type_ == type_t::type());
        return *reinterpret_cast<const type_t *>(this);
    }

    virtual void on_spawn(nomad_game_t &) = 0;
    virtual void on_frame(nomad_game_t &) = 0;
    virtual void on_event(nomad_game_t &, const event::event_t &) = 0;
};

}  // namespace object
