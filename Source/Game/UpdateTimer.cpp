/*
    Copyright (c) 2018-2019 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Game/UpdateTimer.hpp"
using namespace Game;

UpdateTimer::UpdateTimer() :
    m_forwardUpdateCounter(0),
    m_totalUpdateCounter(0),
    m_lastUpdateTime(0.0f)
{
}

UpdateTimer::~UpdateTimer()
{
}

UpdateTimer::UpdateTimer(UpdateTimer&& other) :
    UpdateTimer()
{
    *this = std::move(other);
}

UpdateTimer& UpdateTimer::operator=(UpdateTimer&& other)
{
    Timer::operator=(std::move(other));

    std::swap(m_forwardUpdateCounter, other.m_forwardUpdateCounter);
    std::swap(m_totalUpdateCounter, other.m_totalUpdateCounter);
    std::swap(m_lastUpdateTime, other.m_lastUpdateTime);

    return *this;
}

bool UpdateTimer::Initialize()
{
    LOG() << "Initializing update timer..." << LOG_INDENT();

    // Initialize the base class.
    if(!Timer::Initialize())
    {
        LOG_ERROR() << "Could not initialize base timer class!";
        return false;
    }

    // Set forward update counter to trigger on next update.
    m_forwardUpdateCounter = m_currentTimeCounter;

    // Success!
    return true;
}

void UpdateTimer::Reset()
{
    Timer::Reset();

    // Reset update time counters.
    m_forwardUpdateCounter = m_currentTimeCounter;
    m_totalUpdateCounter = 0;
    m_lastUpdateTime = 0.0f;
}

float UpdateTimer::GetAlphaTime() const
{
    // Calculate accumulated ticks since the last update.
    uint64_t accumulatedUpdateTicks = m_forwardUpdateCounter - m_currentTimeCounter;
    ASSERT(accumulatedUpdateTicks >= 0, "Accumulated update ticks cannot be negative!");

    // Calculate a normalized range between last two updates.
    float accumulatedUpdateTime = accumulatedUpdateTicks * (1.0f / m_timerFrequency);
    float normalizedUpdateAlpha = (m_lastUpdateTime - accumulatedUpdateTime) / m_lastUpdateTime;
    ASSERT(normalizedUpdateAlpha >= 0.0f && normalizedUpdateAlpha <= 1.0f, "Update alpha is not clamped in normal range!");

    return normalizedUpdateAlpha;
}

bool UpdateTimer::Update(float updateTime)
{
    ASSERT(m_timerFrequency != 0, "Timer frequency is invalid!");

    // Convert update time to update ticks.
    uint64_t updateTicks = (uint64_t)(m_timerFrequency * (double)updateTime);

    // Do not allow forward update counter to fall behind the previous tick time.
    // This allows timer with capped delta time to prevent a large number of updates.
    m_forwardUpdateCounter = std::max(m_previousTimeCounter, m_forwardUpdateCounter);

    // Check if we should perform an update.
    if(m_currentTimeCounter >= m_forwardUpdateCounter)
    {
        // Move update counter forward.
        m_forwardUpdateCounter += updateTicks;

        // Track total time of all updates.
        m_totalUpdateCounter += updateTicks;

        // Save the last successful update time.
        m_lastUpdateTime = updateTime;

        // Signal a successful update.
        return true;
    }

    // Wait until we have enough time accumulated.
    return false;
}

float UpdateTimer::GetLastUpdateTime() const
{
    return m_lastUpdateTime;
}

double UpdateTimer::GetTotalUpdateTime() const
{
    // Return the total update time in seconds.
    return m_totalUpdateCounter * (1.0 / m_timerFrequency);
}
