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

        // Ticks the timer to match the current system time.
        void Tick();

        // Ticks the timer to match the current time of another timer.
        void Tick(const Timer& timer);

        // Gets the alpha time in normalized range between last two frames.
        float GetTimeAlpha() const;

        // Gets the delta time in seconds between last two ticks.
        float GetTimeDelta(float maximumDelta = MaximumFloat) const;

        // Gets the current time in seconds.
        double GetCurrentTime() const;

        // Advances frame by a given amount of time.
        // Returns true if frame could be advanced.
        bool AdvanceFrame(float frameTime);

        // Gets the last time length used to advance a frame.
        float GetLastFrameTime() const;

        // Gets the total advanced frame time in seconds.
        double GetTotalFrameTime() const;

    private:
        // Time tracking values.
        uint64_t m_timerFrequency;
        uint64_t m_currentTimeCounter;
        uint64_t m_previousTimeCounter;

        // Frame tracking values.
        uint64_t m_advancedFrameCounter;
        uint64_t m_accumulatedFrameCounter;
        float m_lastAdvancedFrameTime;
    };
}
