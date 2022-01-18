/*
    Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#ifndef PLATFORM_EMSCRIPTEN
    // Use precise time counters on platforms that support it.
    // Emscripten does not implement GLFW function for reading high frequency clock.
    #define USE_PRECISE_TIME_COUNTERS
#endif

/*
    Timer

    Keeps track of time and provides utilities such as automatic
    calculation of delta time between ticks and frame rate measurement.
*/

namespace Platform
{
    class Timer final
    {
        REFLECTION_ENABLE(Timer)

    public:
#ifdef USE_PRECISE_TIME_COUNTERS
        // Time units in integers with constant frequency per second.
        using TimeUnit = uint64_t;
#else
        // Time units in fractional seconds.
        using TimeUnit = double;
#endif

    public:
        Timer();
        ~Timer();

        float Advance(float maxDeltaSeconds = 0.0f);
        void Advance(const Timer& timer);
        void Reset();

        float GetDeltaSeconds() const;
        double GetElapsedSeconds() const;
        TimeUnit GetCurrentTimeUnits() const;
        TimeUnit GetPreviousTimeUnits() const;

        static TimeUnit ConvertToUnits(double seconds);
        static double ConvertToSeconds(TimeUnit units);

    private:
        static TimeUnit ReadClockUnits();
        static TimeUnit ReadClockFrequency();

    private:
        TimeUnit m_currentTimeUnits;
        TimeUnit m_previousTimeUnits;
    };
}

REFLECTION_TYPE(Platform::Timer)
