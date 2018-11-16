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
        ~Timer();

        // Disallow copying.
        Timer(const Timer& other) = delete;
        Timer& operator=(const Timer& other) = delete;

        // Move constructor and operator.
        Timer(Timer&& other);
        Timer& operator=(Timer&& other);

        // Initializes the timer instance.
        bool Initialize();

        // Resets the timer.
        void Reset();

        // Ticks the timer.
        void Tick();

        // Advances frame by a given amount of time.
        // Returns true if frame could be advanced.
        bool AdvanceFrame(float frameTime);

        // Gets the alpha time in normalized range between last two frames.
        float GetTickAlpha(float frameTime);

        // Gets the delta time in seconds between last two ticks.
        float GetTickDelta(float maximumDelta = MaximumFloat);

        // Gets the time in seconds since the application start.
        double GetTickTime() const;

    private:
        // Internal timer values.
        uint64_t m_timerFrequency;
        uint64_t m_currentTimeCounter;
        uint64_t m_previousTimeCounter;
        uint64_t m_advancedFrameCounter;
    };
}
