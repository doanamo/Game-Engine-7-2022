/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "System/Timer.hpp"
using namespace System;

Timer::Timer() = default;
Timer::~Timer() = default;

Timer::CreateResult Timer::Create()
{
    LOG("Creating timer...");
    LOG_SCOPED_INDENT();

    // Create instance.
    auto instance = std::unique_ptr<Timer>(new Timer());

    // Retrieve timer frequency.
    instance->m_timerFrequency = glfwGetTimerFrequency();

    if(instance->m_timerFrequency == 0)
    {
        LOG_ERROR("Could not retrieve correct timer frequency!");
        return Common::Failure(CreateErrors::InvalidFrequencyRetrieved);
    }

    // Retrieve current time counters.
    instance->m_currentTimeCounter = glfwGetTimerValue();
    instance->m_previousTimeCounter = instance->m_currentTimeCounter;

    // Success!
    return Common::Success(std::move(instance));
}

void Timer::Tick(float maximumDelta)
{
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
    // Remember time points of the two last ticks.
    m_previousTimeCounter = timer.m_previousTimeCounter;
    m_currentTimeCounter = timer.m_currentTimeCounter;
}

void Timer::Reset()
{
    // Reset internal timer values.
    m_currentTimeCounter = glfwGetTimerValue();
    m_previousTimeCounter = m_currentTimeCounter;
}

float Timer::GetDeltaTime() const
{
    // Calculate frame time delta between last two ticks in seconds.
    float frameDeltaSeconds = static_cast<float>(GetDeltaTimeCounter() * (1.0 / m_timerFrequency));

    // Return calculated frame delta value.
    return frameDeltaSeconds;
}

double Timer::GetElapsedTime() const
{
    // Return total elapsed time in seconds.
    return m_currentTimeCounter * (1.0 / m_timerFrequency);
}

uint64_t Timer::GetTimerFrequency() const
{
    return m_timerFrequency;
}

uint64_t Timer::GetCurrentTimeCounter() const
{
    return m_currentTimeCounter;
}

uint64_t Timer::GetPreviousTimeCounter() const
{
    return m_previousTimeCounter;
}

uint64_t Timer::GetDeltaTimeCounter() const
{
    // Calculate elapsed time in ticks since the last frame.
    ASSERT(m_currentTimeCounter >= m_previousTimeCounter, "Previous time counter is higher than the current time counter!");
    uint64_t deltaTimeCounter = m_currentTimeCounter - m_previousTimeCounter;

    // Return number of elapsed ticks.
    return deltaTimeCounter;
}
