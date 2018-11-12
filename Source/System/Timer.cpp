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
    m_advancedFrameCounter(0)
{
}

Timer::~Timer()
{
}

Timer::Timer(Timer&& other) : Timer()
{
    *this = std::move(other);
}

Timer& Timer::operator=(Timer&& other)
{
    std::swap(m_timerFrequency, other.m_timerFrequency);
    std::swap(m_currentTimeCounter, other.m_currentTimeCounter);
    std::swap(m_previousTimeCounter, other.m_previousTimeCounter);
    std::swap(m_advancedFrameCounter, other.m_advancedFrameCounter);

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

bool Timer::AdvanceFrame(float frameTime)
{
    ASSERT(m_timerFrequency != 0, "Timer frequency is invalid!");
    ASSERT(m_advancedFrameCounter <= m_currentTimeCounter, "Frame counter is in the future!");

    // Convert frame time to frame ticks.
    uint64_t frameTicks = (uint64_t)(m_timerFrequency * (double)frameTime);

    // Check if we have accumulated enough ticks to advance a frame.
    uint64_t accumlatedTicks = m_currentTimeCounter - m_advancedFrameCounter;

    if(accumlatedTicks >= frameTicks)
    {
        m_advancedFrameCounter += frameTicks;
        return true;
    }

    // Wait until we have enough time accumulated.
    return false;
}

float Timer::GetTickDelta(float maximumDelta)
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

double Timer::GetTickTime() const
{
    ASSERT(m_timerFrequency != 0, "Timer frequency is invalid!");

    // Return time in seconds since the application start.
    return m_currentTimeCounter * (1.0 / m_timerFrequency);
}
