/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "System/Timer.hpp"
using namespace System;

Timer::Timer() = default;
Timer::~Timer() = default;

Timer::InitializeResult Timer::Initialize()
{
    LOG("Initializing timer...");
    LOG_SCOPED_INDENT();

    // Setup initialization guard.
    VERIFY(!m_initialized, "Instance has already been initialized!");
    SCOPE_GUARD_IF(!m_initialized, this->Reset());

    // Retrieve timer frequency.
    m_timerFrequency = glfwGetTimerFrequency();

    if(m_timerFrequency == 0)
    {
        LOG_ERROR("Could not retrieve correct timer frequency!");
        return Failure(InitializeErrors::InvalidFrequencyRetrieved);
    }

    // Retrieve current time counters.
    m_currentTimeCounter = glfwGetTimerValue();
    m_previousTimeCounter = m_currentTimeCounter;

    // Success!
    m_initialized = true;
    return Success();
}

void Timer::Reset()
{
    ASSERT(m_initialized, "Timer has not been initialized!");

    // Reset internal timer values.
    m_currentTimeCounter = glfwGetTimerValue();
    m_previousTimeCounter = m_currentTimeCounter;
}

void Timer::Tick(float maximumDelta)
{
    ASSERT(m_initialized, "Timer has not been initialized!");

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
    ASSERT(m_initialized, "Timer has not been initialized!");

    // Remember time points of the two last ticks.
    m_previousTimeCounter = timer.m_previousTimeCounter;
    m_currentTimeCounter = timer.m_currentTimeCounter;
}

float Timer::GetDeltaTime() const
{
    ASSERT(m_initialized, "Timer has not been initialized!");

    // Calculate frame time delta between last two ticks in seconds.
    float frameDeltaSeconds = static_cast<float>(GetDeltaTimeCounter() * (1.0 / m_timerFrequency));

    // Return calculated frame delta value.
    return frameDeltaSeconds;
}

double Timer::GetElapsedTime() const
{
    ASSERT(m_initialized, "Timer has not been initialized!");

    // Return total elapsed time in seconds.
    return m_currentTimeCounter * (1.0 / m_timerFrequency);
}

uint64_t Timer::GetTimerFrequency() const
{
    ASSERT(m_initialized, "Timer has not been initialized!");
    return m_timerFrequency;
}

uint64_t Timer::GetCurrentTimeCounter() const
{
    ASSERT(m_initialized, "Timer has not been initialized!");
    return m_currentTimeCounter;
}

uint64_t Timer::GetPreviousTimeCounter() const
{
    ASSERT(m_initialized, "Timer has not been initialized!");
    return m_previousTimeCounter;
}

uint64_t Timer::GetDeltaTimeCounter() const
{
    ASSERT(m_initialized, "Timer has not been initialized!");

    // Calculate elapsed time in ticks since the last frame.
    ASSERT(m_currentTimeCounter >= m_previousTimeCounter, "Previous time counter is higher than the current time counter!");
    uint64_t deltaTimeCounter = m_currentTimeCounter - m_previousTimeCounter;

    // Return number of elapsed ticks.
    return deltaTimeCounter;
}

bool System::Timer::IsInitialized() const
{
    return m_initialized;
}
