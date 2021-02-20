/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "System/Precompiled.hpp"
#include "System/Timer.hpp"
using namespace System;

Timer::Timer()
{
    this->Reset();
}

Timer::~Timer() = default;

Timer::CreateResult Timer::Create()
{
    LOG("Creating timer...");
    LOG_SCOPED_INDENT();

    // Create instance.
    auto instance = std::unique_ptr<Timer>(new Timer());

    // Success!
    return Common::Success(std::move(instance));
}

Timer::TimeUnit Timer::ReadClockUnits()
{
    TimeUnit units;

    #ifdef USE_PRECISE_TIME_COUNTERS
        units = glfwGetTimerValue();
    #else
        units = glfwGetTime();
    #endif

    ASSERT(units != 0, "Detected zero timer value! Most likely platform system has not been initialized yet.");

    return units;
}

Timer::TimeUnit Timer::ReadClockFrequency()
{
    TimeUnit frequency;

    #ifdef USE_PRECISE_TIME_COUNTERS
        frequency = glfwGetTimerFrequency();
    #else
        frequency = 1.0;
    #endif

    ASSERT(frequency != 0, "Detected zero timer frequency! Most likely platform system has not been initialized yet.");

    return frequency;
}

Timer::TimeUnit System::Timer::ConvertToUnits(double seconds)
{
    #ifdef USE_PRECISE_TIME_COUNTERS
        return (TimeUnit)(seconds * glfwGetTimerFrequency() + 0.5);
    #else
        return seconds;
    #endif
}

double Timer::ConvertToSeconds(TimeUnit units)
{
    #ifdef USE_PRECISE_TIME_COUNTERS
        return (double)units / glfwGetTimerFrequency();
    #else
        return units;
    #endif
}

float Timer::Advance(float maxDeltaSeconds)
{
    // Remember time points of two last ticks.
    m_previousTimeUnits = m_currentTimeUnits;
    m_currentTimeUnits = ReadClockUnits();

    // Clamp maximum possible delta time by limiting how far back previous time counter can go.
    if(maxDeltaSeconds >= 0.0f)
    {
        TimeUnit tickTimeUnits = std::min(ConvertToUnits((double)maxDeltaSeconds), m_currentTimeUnits);
        m_previousTimeUnits = std::max(m_previousTimeUnits, m_currentTimeUnits - tickTimeUnits);
    }

    // Return time delta in seconds.
    return GetDeltaSeconds();
}

void Timer::Advance(const Timer& timer)
{
    // Copy time points from other timer.
    m_currentTimeUnits = timer.m_currentTimeUnits;
    m_previousTimeUnits = timer.m_previousTimeUnits;
}

void Timer::Reset()
{
    // Reset time point values.
    m_currentTimeUnits = ReadClockUnits();
    m_previousTimeUnits = m_currentTimeUnits;
}

float Timer::GetDeltaSeconds() const
{
    // Calculate elapsed time in ticks since the last frame.
    ASSERT(m_currentTimeUnits >= m_previousTimeUnits, "Previous time is higher than current time!");
    TimeUnit deltaTimeUnits = m_currentTimeUnits - m_previousTimeUnits;

    // Return calculated delta time in seconds.
    return (float)ConvertToSeconds(deltaTimeUnits);
}

double Timer::GetElapsedSeconds() const
{
    return ConvertToSeconds(m_currentTimeUnits);
}

Timer::TimeUnit Timer::GetCurrentTimeUnits() const
{
    return m_currentTimeUnits;
}

Timer::TimeUnit Timer::GetPreviousTimeUnits() const
{
    return m_previousTimeUnits;
}
