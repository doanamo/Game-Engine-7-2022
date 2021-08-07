/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Core/EngineSystem.hpp>

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
    class Timer final : public Core::EngineSystem
    {
        REFLECTION_ENABLE(Timer, Core::EngineSystem)

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
        ~Timer() override;

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
        bool OnAttach(const Core::EngineSystemStorage& engineSystems) override;
        void OnBeginFrame() override;

        static TimeUnit ReadClockUnits();
        static TimeUnit ReadClockFrequency();

    private:
        TimeUnit m_currentTimeUnits;
        TimeUnit m_previousTimeUnits;

        float m_maxUpdateDelta = 1.0f;
    };
}

REFLECTION_TYPE(System::Timer, Core::EngineSystem)
