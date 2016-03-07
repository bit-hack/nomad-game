#pragma once

// simple reference counted object
template <typename type_t>
struct ref_t {

    // ctor
    ref_t()
        : obj_(nullptr)
    {
    }

    // ctor
    ref_t(type_t * obj)
        : obj_(obj)
    {
        inc_ref();
    }

    // copy ctor
    ref_t(const ref_t & ref)
        : obj_(ref.obj_)
    {
        inc_ref();
    }

    // move ctor
    ref_t(ref_t && ref)
        : obj_(ref.obj_)
    {
        ref.obj_ = nullptr;
    }

    // dtor
    ~ref_t()
    {
        dispose();
    }

    // dispose this reference
    void dispose()
    {
        if (!obj_)
            return;
        if (--(obj_->ref_) > 0)
            return;
//        delete obj_;
        obj_ = nullptr;
    }

    // index the data
    type_t * operator->() const
    {
        assert(obj_);
        return obj_;
    }

    // reasign data
    void operator=(const ref_t & other)
    {
        dispose();
        obj_ = other.obj_;
        inc_ref();
    }

    // verify reference exists
    operator bool() const
    {
        return obj_ != nullptr;
    }

   protected:
    // increment the reference
    void inc_ref()
    {
        if (obj_)
            ++(obj_->ref_);
    }

    type_t * obj_;
};
