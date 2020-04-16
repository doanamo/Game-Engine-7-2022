/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <limits>

/*
    Timer

    Keeps track of time and provides utilities such as automatic
    calculation of delta time between ticks and frame rate measurement.

    void ExampleSystemTimer()
    {
        // Create a timer instance.
        System::Timer timer;
        timer.Initialize();

        // Run a loop that will measure delta time.
        while(true)
        {
            // Calculate delta time between two last ticks.
            float dt = timer.CalculateTickDelta();
            Log() << "Current delta time: " << dt;

            // Perform some calculations over a frame and tick the timer.
            Sleep(1000);
            timer.Tick();
        }
    }
*/

namespace System
{
    class Timer final : private NonCopyable, public Resettable<Timer>
    {
    public:
        enum class InitializeErrors
        {
            InvalidFrequencyRetrieved,
        };

        using InitializeResult = Result<void, InitializeErrors>;

    public:
        Timer();
        ~Timer();

        InitializeResult Initialize();
        void Reset();

        void Tick(float maximumDelta = 0.0f);
        void Tick(const Timer& timer);

        float GetDeltaTime() const;
        double GetElapsedTime() const;

        uint64_t GetTimerFrequency() const;
        uint64_t GetCurrentTimeCounter() const;
        uint64_t GetPreviousTimeCounter() const;
        uint64_t GetDeltaTimeCounter() const;

        bool IsInitialized() const;

    private:
        uint64_t m_timerFrequency = 0;
        uint64_t m_currentTimeCounter = 0;
        uint64_t m_previousTimeCounter = 0;
        bool m_initialized = false;
    };
}
