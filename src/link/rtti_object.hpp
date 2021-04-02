#pragma once

#include "rtti.hpp"

namespace link
{
    struct Rtti;

    struct RttiObject
    {
        virtual ~RttiObject() {}

        static const Rtti TYPE;
        inline virtual const Rtti& get_type() const;
        inline bool is_exactly(const Rtti& type) const;
        inline bool is_derived_of(const Rtti& base_type) const;
        inline bool is_base_of(const Rtti& child_type) const;
        inline bool is_exactly_type_of(const RttiObject* obj) const;
        inline bool is_derived_type_of(const RttiObject* obj) const;
    };

    template <class T> T* rtti_static_cast(RttiObject* obj);
    template <class T> const T* rtti_static_cast(const RttiObject* obj);
    template <class T> T* rtti_dynamic_cast(RttiObject* obj);
    template <class T> const T* rtti_dynamic_cast(const RttiObject* obj);


    const Rtti& RttiObject::get_type() const
    {
        return TYPE;
    }

    bool RttiObject::is_exactly(const Rtti& type) const
    {
        return get_type().is_exactly(type);
    }

    bool RttiObject::is_derived_of(const Rtti& base_type) const
    {
        return get_type().is_derived(base_type);
    }

    bool RttiObject::is_base_of(const Rtti& child_type) const
    {
        return child_type.is_derived(get_type());
    }

    bool RttiObject::is_exactly_type_of(const RttiObject* comp) const
    {
        return comp && get_type().is_exactly(comp->get_type());
    }

    bool RttiObject::is_derived_type_of(const RttiObject* comp) const
    {
        return comp && get_type().is_derived(comp->get_type());
    }


    template <class T>
    T* rtti_static_cast(RttiObject* obj)
    {
        return (T*)obj;
    }

    template <class T>
    const T* rtti_static_cast(const RttiObject* obj)
    {
        return (const T*)obj;
    }

    template <class T>
    T* rtti_dynamic_cast(RttiObject* obj)
    {
        return obj && obj->is_derived(T::TYPE) ? (T*)obj : 0;
    }

    template <class T>
    const T* rtti_dynamic_cast(const RttiObject* obj)
    {
        return obj && obj->is_derived(T::TYPE) ? (const T*)obj : 0;
    }
}