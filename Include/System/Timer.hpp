/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    System Timer

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
    // Timer class.
    class Timer
    {
    public:
        // Constant values.
        static constexpr float MaximumFloat = std::numeric_limits<float>::max();

    public:
        Timer();
        virtual ~Timer();

        // Disallow copying.
        Timer(const Timer& other) = delete;
        Timer& operator=(const Timer& other) = delete;

        // Move constructor and operator.
        Timer(Timer&& other);
        Timer& operator=(Timer&& other);

        // Initializes the timer instance.
        virtual bool Initialize();

        // Resets the timer.
        virtual void Reset();

        // Ticks the timer to match the current system time.
        void Tick();

        // Ticks the timer to match the current time of another timer.
        void Tick(const Timer& timer);

        // Gets the delta time in seconds between last two ticks.
        float GetDeltaTime(float maximumDelta = MaximumFloat) const;

        // Gets the current time in seconds.
        double GetSystemTime() const;

    protected:
        // Time tracking values.
        uint64_t m_timerFrequency;
        uint64_t m_currentTimeCounter;
        uint64_t m_previousTimeCounter;
    };
}
