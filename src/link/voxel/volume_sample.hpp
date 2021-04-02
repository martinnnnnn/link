#pragma once

#include "link/input.hpp"

namespace link
{
    struct Sample
    {
        Sample(i8 value) : value(value) {}
        Sample() : Sample(0) {}

        i8 value;
    };
}


