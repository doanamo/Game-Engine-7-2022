/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <limits>

/*
    Timer

    Keeps track of time and provides utilities such as automatic
    calculation of delta time between ticks and frame rate measurement.
*/

namespace System
{
    class Timer final : private NonCopyable
    {
    public:
        enum class CreateErrors
        {
            InvalidFrequencyRetrieved,
        };

        using CreateResult = Result<std::unique_ptr<Timer>, CreateErrors>;
        static CreateResult Create();

    public:
        ~Timer();

        void Tick(float maximumDelta = 0.0f);
        void Tick(const Timer& timer);
        void Reset();

        float GetDeltaTime() const;
        double GetElapsedTime() const;

        uint64_t GetTimerFrequency() const;
        uint64_t GetCurrentTimeCounter() const;
        uint64_t GetPreviousTimeCounter() const;
        uint64_t GetDeltaTimeCounter() const;

    private:
        Timer();

    private:
        uint64_t m_timerFrequency = 0;
        uint64_t m_currentTimeCounter = 0;
        uint64_t m_previousTimeCounter = 0;
    };
}
