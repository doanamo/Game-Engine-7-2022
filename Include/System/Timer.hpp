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
    class Timer : private NonCopyable
    {
    public:
        static constexpr float MaximumFloat = std::numeric_limits<float>::max();

    public:
        Timer() = default;
        virtual ~Timer() = default;

        Timer(Timer&& other);
        Timer& operator=(Timer&& other);

        virtual bool Initialize();
        virtual void Reset();

        void Tick(float maximumDelta = 0.0f);
        void Tick(const Timer& timer);

        uint64_t GetDeltaTicks() const;
        float GetDeltaTime() const;
        double GetSystemTime() const;

    protected:
        uint64_t m_timerFrequency = 0;
        uint64_t m_currentTimeCounter = 0;
        uint64_t m_previousTimeCounter = 0;
    };
}
