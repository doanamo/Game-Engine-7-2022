/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Game/UpdateTimer.hpp"
using namespace Game;

UpdateTimer::UpdateTimer() = default;
UpdateTimer::~UpdateTimer() = default;

UpdateTimer::CreateResult UpdateTimer::Create()
{
    LOG("Creating update timer...");
    LOG_SCOPED_INDENT();

    // Create instance.
    auto instance = std::unique_ptr<UpdateTimer>(new UpdateTimer());

    // Create timer.
    instance->m_timer = System::Timer::Create().UnwrapOr(nullptr);
    if(instance->m_timer == nullptr)
    {
        LOG_ERROR("Could not create timer!");
        return Common::Failure();
    }

    // Set forward update counter to trigger on next update.
    instance->m_forwardUpdateCounter = instance->m_timer->GetCurrentTimeCounter();

    // Success!
    return Common::Success(std::move(instance));
}

void UpdateTimer::Reset()
{
    m_timer->Reset();

    // Reset update time counters.
    m_forwardUpdateCounter = m_timer->GetCurrentTimeCounter();
    m_totalUpdateCounter = 0;
    m_lastUpdateTime = 0.0f;
}

float UpdateTimer::GetAlphaTime() const
{
    // Calculate accumulated ticks since the last update.
    uint64_t accumulatedUpdateTicks = m_forwardUpdateCounter - m_timer->GetCurrentTimeCounter();
    ASSERT(accumulatedUpdateTicks >= 0, "Accumulated update ticks cannot be negative!");

    // Calculate a normalized range between last two updates.
    float accumulatedUpdateTime = accumulatedUpdateTicks * (1.0f / m_timer->GetTimerFrequency());
    float normalizedUpdateAlpha = (m_lastUpdateTime - accumulatedUpdateTime) / m_lastUpdateTime;
    ASSERT(normalizedUpdateAlpha >= 0.0f && normalizedUpdateAlpha <= 1.0f, "Update alpha is not clamped in normal range!");

    // Return alpha time in normalized range between last two updates.
    return normalizedUpdateAlpha;
}

void UpdateTimer::Tick(const System::Timer& timer)
{
    m_timer->Tick(timer);
}

bool UpdateTimer::Update(float updateTime)
{
    // Convert update time to update ticks.
    uint64_t updateTicks = (uint64_t)(m_timer->GetTimerFrequency() * (double)updateTime + 0.5);

    // Do not allow forward update counter to fall behind the previous tick time.
    // This allows timer with capped delta time to prevent a large number of updates.
    m_forwardUpdateCounter = std::max(m_timer->GetPreviousTimeCounter(), m_forwardUpdateCounter);

    // Check if we should perform an update.
    if(m_timer->GetCurrentTimeCounter() >= m_forwardUpdateCounter)
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
    // Return false to indicate that update did not occur.
    return false;
}

float UpdateTimer::GetLastUpdateTime() const
{
    return m_lastUpdateTime;
}

double UpdateTimer::GetTotalUpdateTime() const
{
    // Return the total update time in seconds.
    return m_totalUpdateCounter * (1.0 / m_timer->GetTimerFrequency());
}
