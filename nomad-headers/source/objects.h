#pragma once

#include "uuid.h"
#include "../../nomad-util/source/geometric.h"

namespace object {

// base class for all game object that exist within the world.
struct object_t {
    uuid::object_uuid_t uuid_;    // object uuid
    uint32_t      type_;    // object type
    geom::vec2i_t pos_[2];  // old/new pos
    uint32_t      ref_;     // reference count

                            // checked downcast
    template <typename type_t>
    type_t & cast()
    {
        assert(type_==type_t::type());
        return *reinterpret_cast<type_t *>(this);
    }

    // checked downcast
    template <typename type_t>
    const type_t & cast() const
    {
        assert(type_==type_t::type());
        return *reinterpret_cast<const type_t *>(this);
    }

    // inherit to add more data ...
};

// reference counted object container
struct object_ref_t {

    object_ref_t(object_t * obj)
        : obj_(obj)
    {
        ++(obj->ref_);
    }

    object_ref_t(object_ref_t & ref)
        : obj_(ref.obj_)
    {
        if (obj_) {
            ++(obj_->ref_);
        }
    }

    object_ref_t(object_ref_t && ref)
        : obj_(ref.obj_)
    {
    }

    ~object_ref_t()
    {
        dispose();
    }

    void dispose()
    {
        if (obj_) {
            if (--(obj_->ref_)==0) {
                delete obj_;
            }
            obj_ = nullptr;
        }
    }

    object_t & operator->()
    {
        assert(obj_);
        return *obj_;
    }

    const object_t & operator->() const
    {
        assert(obj_);
        return *obj_;
    }

    operator bool() const
    {
        return obj_!=nullptr;
    }

protected:
    object_t * obj_;
};

enum type_t {
    e_unknown,
    e_tree,
    e_villager,
};

struct villager_t : public object_t {
    enum role_t {
        e_idle,
        e_farmer,
        e_lumberjack,
        e_swordsman,
        e_archer,
        e_wizard
    };

    uuid::player_uuid_t player_;
    geom::vec2i_t        dest_;
    role_t               role_;

    static type_t type()
    {
        return type_t::e_villager;
    }
};

}  // namespace object
