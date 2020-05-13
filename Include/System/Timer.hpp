/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <limits>

#ifndef __EMSCRIPTEN__
    // Use precise time counters on platforms that support it.
    // Emscripten does not implement GLFW function for reading high frequency clock.
    #define USE_PRECISE_TIME_COUNTERS
#endif

/*
    Timer

    Keeps track of time and provides utilities such as automatic
    calculation of delta time between ticks and frame rate measurement.
*/

namespace System
{
    class Timer final : private Common::NonCopyable
    {
    public:
        #ifdef USE_PRECISE_TIME_COUNTERS
            // Time units in integers with constant frequency per second.
            using TimeUnit = uint64_t;
        #else
            // Time units in fractional seconds.
            using TimeUnit = double;
        #endif

        using CreateResult = Common::Result<std::unique_ptr<Timer>, void>;
        static CreateResult Create();

    public:
        ~Timer();

        void Tick(float maxDeltaSeconds = 0.0f);
        void Tick(const Timer& timer);
        void Reset();

        float GetDeltaSeconds() const;
        double GetElapsedSeconds() const;
        TimeUnit GetCurrentTimeUnits() const;
        TimeUnit GetPreviousTimeUnits() const;

        static TimeUnit ConvertToUnits(double seconds);
        static double ConvertToSeconds(TimeUnit units);

    private:
        Timer();

        static TimeUnit ReadClockUnits();
        static TimeUnit ReadClockFrequency();

    private:
        TimeUnit m_currentTimeUnits;
        TimeUnit m_previousTimeUnits;
    };
}
