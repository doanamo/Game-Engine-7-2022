/*
    Copyright (c) 2018-2019 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "System/Timer.hpp"
using namespace System;

Timer::Timer() :
    m_timerFrequency(0),
    m_currentTimeCounter(0),
    m_previousTimeCounter(0)
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

    // Retrieve current time counters.
    m_currentTimeCounter = glfwGetTimerValue();
    m_previousTimeCounter = m_currentTimeCounter;

    // Success!
    return true;
}

void Timer::Reset()
{
    ASSERT(m_timerFrequency != 0, "Timer frequency is invalid!");

    // Reset internal timer values.
    m_currentTimeCounter = glfwGetTimerValue();
    m_previousTimeCounter = m_currentTimeCounter;
}

void Timer::Tick(float maximumDelta)
{
    ASSERT(m_timerFrequency != 0, "Timer frequency is invalid!");

    // Remember time points of the two last ticks.
    m_previousTimeCounter = m_currentTimeCounter;
    m_currentTimeCounter = glfwGetTimerValue();

    // Clamp maximum possible delta time by limiting how far back previous time counter can go.
    if(maximumDelta >= 0.0f)
    {
        uint64_t maximumTicks = std::min((uint64_t)((double)maximumDelta * m_timerFrequency), m_currentTimeCounter);
        m_previousTimeCounter = std::max(m_previousTimeCounter, m_currentTimeCounter - maximumTicks);
    }
}

void Timer::Tick(const Timer& timer)
{
    ASSERT(m_timerFrequency != 0, "Timer frequency is invalid!");

    // Remember time points of the two last ticks.
    m_previousTimeCounter = timer.m_previousTimeCounter;
    m_currentTimeCounter = timer.m_currentTimeCounter;
}

uint64_t Timer::GetDeltaTicks() const
{
    ASSERT(m_timerFrequency != 0, "Timer frequency is invalid!");

    // Calculate elapsed time in ticks since the last frame.
    ASSERT(m_currentTimeCounter >= m_previousTimeCounter, "Previous time counter is higher than the current time counter!");
    uint64_t elapsedTimeCounter = m_currentTimeCounter - m_previousTimeCounter;

    // Return the number of elapsed ticks.
    return elapsedTimeCounter;
}

float Timer::GetDeltaTime() const
{
    ASSERT(m_timerFrequency != 0, "Timer frequency is invalid!");

    // Calculate frame time delta between last two ticks in seconds.
    float frameDeltaSeconds = static_cast<float>(GetDeltaTicks() * (1.0 / m_timerFrequency));

    // Return calculated frame delta value.
    return frameDeltaSeconds;
}

double Timer::GetSystemTime() const
{
    ASSERT(m_timerFrequency != 0, "Timer frequency is invalid!");

    // Return the current time in seconds.
    return m_currentTimeCounter * (1.0 / m_timerFrequency);
}
