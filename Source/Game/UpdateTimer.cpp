/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Game/UpdateTimer.hpp"
using System::Timer;
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
    instance->m_timer = Timer::Create().UnwrapOr(nullptr);
    if(instance->m_timer == nullptr)
    {
        LOG_ERROR("Could not create timer!");
        return Common::Failure();
    }

    // Set forward update counter to trigger on next update.
    instance->m_forwardUpdateTimeUnits = instance->m_timer->GetCurrentTimeUnits();

    // Success!
    return Common::Success(std::move(instance));
}

void UpdateTimer::Reset()
{
    m_timer->Reset();

    // Reset update time units.
    m_forwardUpdateTimeUnits = m_timer->GetCurrentTimeUnits();
    m_totalUpdateTimeUnits = 0;
    m_lastUpdateSeconds = 0.0f;
}

float UpdateTimer::GetAlphaSeconds() const
{
    // Calculate accumulated time units since the last update.
    TimeUnit accumulatedUpdateUnits = m_forwardUpdateTimeUnits - m_timer->GetCurrentTimeUnits();
    ASSERT(accumulatedUpdateUnits >= 0, "Accumulated update units cannot be negative!");

    // Calculate normalized range between last two updates.
    float accumulatedUpdateSeconds = (float)Timer::ConvertToSeconds(accumulatedUpdateUnits);
    float normalizedUpdateAlpha = (m_lastUpdateSeconds - accumulatedUpdateSeconds) / m_lastUpdateSeconds;
    ASSERT(normalizedUpdateAlpha >= 0.0f && normalizedUpdateAlpha <= 1.0f,
        "Update alpha is not clamped in normal range!");

    // Return alpha time in normalized range between last two updates.
    return normalizedUpdateAlpha;
}

void UpdateTimer::Advance(const Timer& timer)
{
    m_timer->Advance(timer);
}

bool UpdateTimer::Update(float updateSeconds)
{
    // Convert update seconds to update units.
    TimeUnit updateUnits = Timer::ConvertToUnits((double)updateSeconds);

    // Do not allow forward update counter to fall behind the previous tick time.
    // This allows timer with capped delta time to prevent a large number of updates.
    m_forwardUpdateTimeUnits = std::max(m_timer->GetPreviousTimeUnits(), m_forwardUpdateTimeUnits);

    // Check if we should perform an update.
    if(m_timer->GetCurrentTimeUnits() >= m_forwardUpdateTimeUnits)
    {
        // Move update counter forward.
        m_forwardUpdateTimeUnits += updateUnits;

        // Track total time of all updates.
        m_totalUpdateTimeUnits += updateUnits;

        // Save last successful update time.
        m_lastUpdateSeconds = updateSeconds;

        // Signal successful update.
        return true;
    }

    // Wait until we have enough time accumulated.
    // Return false to indicate that update did not occur.
    return false;
}

double UpdateTimer::GetTotalUpdateSeconds() const
{
    return Timer::ConvertToSeconds(m_totalUpdateTimeUnits);
}

float UpdateTimer::GetLastUpdateSeconds() const
{
    return m_lastUpdateSeconds;
}
