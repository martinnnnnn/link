#pragma once


#include <time.h>
#include <stdlib.h>
#include <random>

#include <fmt/ostream.h>

#include "types.hpp"

namespace link
{
    struct CRandom
    {
        static inline void initialize()
        {
            srand(u32(time(NULL)));
        }

        static inline u32 range(u32 min, u32 max)
        {
            return min + (rand() % (max - min));
        }

        static inline u32 range_0X(u32 max)
        {
            return range(0, max);
        }

        static inline f32 range_f01()
        {
            return f32(rand() % 100) / 100.0f;
        }

        static inline f32 range_f(f32 min, f32 max)
        {
            f32 result = range_f01() * (max - min) + min;
            //fmt::print("[{} - {}] -> {}\n", min, max, result);
            return result;
        }

        static inline f32 range_f0X(f32 max)
        {
            return range_f(0.0f, max);
        }
    };

    //struct Random
    //{
    //	std::random_device rd;
    //	std::default_random_engine engine;

    //	Random()			: engine(std::random_device()) {}
    //	Random(u32 seed)	: engine(seed) {}

    //	inline u32 range(u32 min, u32 max)
    //	{
    //		std::uniform_int_distribution<u32> distr(min, max);
    //		return distr(engine);
    //	}

    //	inline u32 range_0X(u32 max)
    //	{
    //		return range(0, max);
    //	}

    //	inline f32 range_f(f32 min, f32 max)
    //	{
    //		std::uniform_int_distribution<f32> distr(min, max);
    //		return distr(engine);
    //	}

    //	inline f32 range_f01()
    //	{
    //		return range_f(0.0f, 1.0f);
    //	}

    //	inline f32 range_f0X(f32 max)
    //	{
    //		return range_f(0.0f, max);
    //	}
    //};
}