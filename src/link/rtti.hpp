#pragma once

#include <string>

namespace link
{
    class Rtti
    {
    public:
        Rtti(const std::string name, const Rtti* base);
        ~Rtti();

        const std::string get_name() const;

        bool is_exactly(const Rtti& type) const;
        bool is_derived(const Rtti& base_type) const;

    private:
        std::string name;
        const Rtti* base;
    };

}

#define LINK_DECLARE_RTTI \
public: \
    static const link::Rtti TYPE; \
    virtual const link::Rtti& get_type () const { return TYPE; }

#define LINK_IMPLEMENT_RTTI(nsname,classname,baseclassname) \
    const link::Rtti classname::TYPE(#nsname"."#classname,&baseclassname::TYPE)