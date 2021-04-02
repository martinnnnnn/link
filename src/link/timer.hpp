#pragma once

#include "SDL.h"

#include "types.hpp"
#include "singleton.hpp"

namespace link
{
    struct Timer
    {
        u64 NOW;
        u64 LAST;
        f64 dt_ms;
        f64 dt_s;
        f64 total_time_ms;
        f64 total_time_s;

        Timer()
            : NOW(0)
            , LAST(0)
            , dt_ms(0)
            , dt_s(0)
            , total_time_ms(0.0)
            , total_time_s(0.0)
        {}

        void start()
        {
            NOW = SDL_GetPerformanceCounter();
            LAST = 0;
            dt_ms = 0;
            dt_s = 0;
            total_time_ms = 0.0;
            total_time_s = 0.0;
        }

        void update()
        {
            LAST = NOW;
            NOW = SDL_GetPerformanceCounter();
            dt_ms = (double)((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency());
            dt_s = dt_ms * 0.001;
            total_time_ms += dt_ms;
            total_time_s += dt_s;
        }
    };

    struct GlobalTimer : Singleton<GlobalTimer>
    {
        Timer timer;

        void start()
        {
            timer.start();
        }

        void update()
        {
            timer.update();
        }
    };
}

#define LINK_TIME link::GlobalTimer::get()

