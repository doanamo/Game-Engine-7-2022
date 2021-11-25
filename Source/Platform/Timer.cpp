/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Platform/Timer.hpp"
using namespace Platform;

Timer::Timer()
{
    Reset();
}

Timer::~Timer() = default;

Timer::TimeUnit Timer::ReadClockUnits()
{
#ifdef USE_PRECISE_TIME_COUNTERS
    TimeUnit units = glfwGetTimerValue();
#else
    TimeUnit units = glfwGetTime();
#endif

    ASSERT(units != 0, "Detected zero timer value! Most likely platform system has not been initialized yet.");
    return units;
}

Timer::TimeUnit Timer::ReadClockFrequency()
{
#ifdef USE_PRECISE_TIME_COUNTERS
    TimeUnit frequency = glfwGetTimerFrequency();
#else
    TimeUnit frequency = 1.0;
#endif

    ASSERT(frequency != 0, "Detected zero timer frequency! Most likely platform system has not been initialized yet.");
    return frequency;
}

Timer::TimeUnit Platform::Timer::ConvertToUnits(double seconds)
{
#ifdef USE_PRECISE_TIME_COUNTERS
    return static_cast<TimeUnit>(seconds * glfwGetTimerFrequency() + 0.5);
#else
    return seconds;
#endif
}

double Timer::ConvertToSeconds(TimeUnit units)
{
#ifdef USE_PRECISE_TIME_COUNTERS
    return static_cast<double>(units) / glfwGetTimerFrequency();
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
        TimeUnit tickTimeUnits = std::min(ConvertToUnits(
            static_cast<double>(maxDeltaSeconds)), m_currentTimeUnits);
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
    return static_cast<float>(ConvertToSeconds(deltaTimeUnits));
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
