/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "System/Timer.hpp"
using namespace System;

Timer::Timer() :
    m_timerFrequency(glfwGetTimerFrequency()),
    m_currentTimeCounter(glfwGetTimerValue()),
    m_previousTimeCounter(m_currentTimeCounter),
    m_maxFrameDeltaSeconds(std::numeric_limits<float>::max())
{
    // Check if the timer's frequency is valid.
    // Assertion's failure may indicate improperly initialized GLFW library.
    VERIFY(m_timerFrequency != 0, "Failed to retrieve a valid timer frequency!");
}

Timer::~Timer()
{
}

void Timer::Reset()
{
    ASSERT(m_timerFrequency != 0, "Timer frequency is invalid!");

    // Reset internal timer values.
    m_currentTimeCounter = glfwGetTimerValue();
    m_previousTimeCounter = m_currentTimeCounter;
}

void Timer::Tick()
{
    ASSERT(m_timerFrequency != 0, "Timer frequency is invalid!");

    // Remember time points of the two last ticks.
    m_previousTimeCounter = m_currentTimeCounter;
    m_currentTimeCounter = glfwGetTimerValue();
}

float Timer::CalculateFrameDelta()
{
    ASSERT(m_timerFrequency != 0, "Timer frequency is invalid!");

    // Calculate elapsed time since the last frame.
    ASSERT(m_currentTimeCounter >= m_previousTimeCounter, "Previous time counter is higher than the current time counter!");
    uint64_t elapsedTimeCounter = m_currentTimeCounter - m_previousTimeCounter;

    // Calculate frame time delta between last two ticks in seconds.
    float frameDeltaSeconds = static_cast<float>(elapsedTimeCounter * (1.0 / m_timerFrequency));

    // Clamp delta value between 0.0f and set maximum value.
    frameDeltaSeconds = Utility::Clamp(frameDeltaSeconds, 0.0f, m_maxFrameDeltaSeconds);

    // Return calculated frame delta value.
    return frameDeltaSeconds;
}

double Timer::GetCurrentTime() const
{
    ASSERT(m_timerFrequency != 0, "Timer frequency is invalid!");

    // Return time in seconds since the application start.
    return m_currentTimeCounter * (1.0 / m_timerFrequency);
}

void Timer::SetMaxFrameDelta(float value)
{
    ASSERT(m_timerFrequency != 0, "Timer frequency is invalid!");

    m_maxFrameDeltaSeconds = value;
}

float Timer::GetMaxFrameDelta() const
{
    ASSERT(m_timerFrequency != 0, "Timer frequency is invalid!");

    return m_maxFrameDeltaSeconds;
}
