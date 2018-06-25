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

        // Run a loop that will measure delta time.
        while(true)
        {
            // Calculate delta time between two last ticks.
            float dt = timer.CalculateFrameDelta();
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
        Timer();
        ~Timer();

        // Resets the timer.
        void Reset();

        // Ticks the timer.
        void Tick();

        // Calculates a frame delta time in seconds between last two ticks.
        float CalculateFrameDelta();

        // Sets the maximum frame delta in seconds that can be returned.
        void SetMaxFrameDelta(float value);

        // Gets the maximum frame delta in seconds that can be returned.
        float GetMaxFrameDelta() const;

    private:
        // Internal timer values.
        uint64_t m_timerFrequency;
        uint64_t m_currentTimeCounter;
        uint64_t m_previousTimeCounter;

        // Maximum accumulated frame delta.
        float m_maxFrameDeltaSeconds;
    };
}
