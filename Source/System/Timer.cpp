/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "System/Timer.hpp"
using namespace System;

Timer::Timer() :
    m_timerFrequency(0),
    m_currentTimeCounter(0),
    m_previousTimeCounter(0),
    m_advancedFrameCounter(0),
    m_accumulatedFrameCounter(0),
    m_lastAdvancedFrameTime(0.0f)
{
}

Timer::~Timer()
{
}

Timer::Timer(Timer&& other) :
    Timer()
{
    *this = std::move(other);
}

Timer& Timer::operator=(Timer&& other)
{
    std::swap(m_timerFrequency, other.m_timerFrequency);
    std::swap(m_currentTimeCounter, other.m_currentTimeCounter);
    std::swap(m_previousTimeCounter, other.m_previousTimeCounter);

    std::swap(m_advancedFrameCounter, other.m_advancedFrameCounter);
    std::swap(m_accumulatedFrameCounter, other.m_accumulatedFrameCounter);
    std::swap(m_lastAdvancedFrameTime, other.m_lastAdvancedFrameTime);

    return *this;
}

bool Timer::Initialize()
{
    LOG() << "Initializing timer..." << LOG_INDENT();

    // Check if instance is already initialized.
    ASSERT(m_timerFrequency == 0, "Time instance has already been initialized!");

    // Retrieve timer frequency.
    m_timerFrequency = glfwGetTimerFrequency();

    if(m_timerFrequency == 0)
    {
        LOG_ERROR() << "Could not retrieve timer frequency!";
        return false;
    }

    // Retrieve current times.
    m_currentTimeCounter = glfwGetTimerValue();
    m_previousTimeCounter = m_currentTimeCounter;
    m_advancedFrameCounter = m_currentTimeCounter;

    // Success!
    return true;
}

void Timer::Reset()
{
    ASSERT(m_timerFrequency != 0, "Timer frequency is invalid!");

    // Reset internal timer values.
    m_currentTimeCounter = glfwGetTimerValue();
    m_previousTimeCounter = m_currentTimeCounter;
    m_advancedFrameCounter = m_currentTimeCounter;
}

void Timer::Tick()
{
    ASSERT(m_timerFrequency != 0, "Timer frequency is invalid!");

    // Remember time points of the two last ticks.
    m_previousTimeCounter = m_currentTimeCounter;
    m_currentTimeCounter = glfwGetTimerValue();
}

void Timer::Tick(const Timer& timer)
{
    ASSERT(m_timerFrequency != 0, "Timer frequency is invalid!");

    // Remember time points of the two last ticks.
    m_previousTimeCounter = m_currentTimeCounter;
    m_currentTimeCounter = timer.m_currentTimeCounter;
}

float Timer::GetTimeDelta(float maximumDelta) const
{
    ASSERT(m_timerFrequency != 0, "Timer frequency is invalid!");

    // Calculate elapsed time since the last frame.
    ASSERT(m_currentTimeCounter >= m_previousTimeCounter, "Previous time counter is higher than the current time counter!");
    uint64_t elapsedTimeCounter = m_currentTimeCounter - m_previousTimeCounter;

    // Calculate frame time delta between last two ticks in seconds.
    float frameDeltaSeconds = static_cast<float>(elapsedTimeCounter * (1.0 / m_timerFrequency));

    // Clamp delta value between 0.0f and set maximum value.
    frameDeltaSeconds = Utility::Clamp(frameDeltaSeconds, 0.0f, maximumDelta);

    // Return calculated frame delta value.
    return frameDeltaSeconds;
}

float Timer::GetTimeAlpha() const
{
    // Calculate accumulated ticks since the last frame.
    uint64_t accumulatedFrameTicks = m_advancedFrameCounter - m_currentTimeCounter;
    ASSERT(accumulatedFrameTicks >= 0, "Accumulated ticks cannot be negative!");

    // Calculate a normalized range between last two frames.
    float accumulatedFrameTime = accumulatedFrameTicks * (1.0f / m_timerFrequency);
    float normalizedFrameAlpha = (m_lastAdvancedFrameTime - accumulatedFrameTime) / m_lastAdvancedFrameTime;
    ASSERT(normalizedFrameAlpha >= 0.0f && normalizedFrameAlpha <= 1.0f, "Time alpha is not clamped in normal range!");

    return normalizedFrameAlpha;
}

double Timer::GetCurrentTime() const
{
    ASSERT(m_timerFrequency != 0, "Timer frequency is invalid!");

    // Return the current time in seconds.
    return m_currentTimeCounter * (1.0 / m_timerFrequency);
}

bool Timer::AdvanceFrame(float frameTime)
{
    ASSERT(m_timerFrequency != 0, "Timer frequency is invalid!");

    // Check if we should advance the frame.
    if(m_currentTimeCounter >= m_advancedFrameCounter)
    {
        // Convert frame time to frame ticks.
        uint64_t frameTicks = (uint64_t)(m_timerFrequency * (double)frameTime);

        // Advance counter for the next frame.
        m_advancedFrameCounter += frameTicks;

        // Track accumulated time of all advanced frame.
        m_accumulatedFrameCounter += frameTicks;

        // Save the last advanced frame time.
        m_lastAdvancedFrameTime = frameTime;

        return true;
    }

    // Wait until we have enough time accumulated.
    return false;
}

float Timer::GetLastFrameTime() const
{
    return m_lastAdvancedFrameTime;
}

double Timer::GetTotalFrameTime() const
{
    // Return the advanced time in seconds.
    return m_accumulatedFrameCounter * (1.0 / m_timerFrequency);
}
