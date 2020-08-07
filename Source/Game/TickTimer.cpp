/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Game/TickTimer.hpp"
using System::Timer;
using namespace Game;

TickTimer::TickTimer() = default;
TickTimer::~TickTimer() = default;

TickTimer::CreateResult TickTimer::Create()
{
    LOG("Creating tick timer...");
    LOG_SCOPED_INDENT();

    // Create instance.
    auto instance = std::unique_ptr<TickTimer>(new TickTimer());

    // Create timer.
    instance->m_timer = Timer::Create().UnwrapOr(nullptr);
    if(instance->m_timer == nullptr)
    {
        LOG_ERROR("Could not create timer!");
        return Common::Failure();
    }

    // Set forward tick counter to trigger on next tick.
    instance->m_forwardTickTimeUnits = instance->m_timer->GetCurrentTimeUnits();

    // Success!
    return Common::Success(std::move(instance));
}

void TickTimer::Reset()
{
    m_timer->Reset();

    // Reset tick time units.
    m_forwardTickTimeUnits = m_timer->GetCurrentTimeUnits();
    m_totalTickTimeUnits = 0;
    m_lastTickSeconds = 0.0f;
}

void TickTimer::SetTickSeconds(float tickTime)
{
    m_tickSeconds = tickTime;
}

void TickTimer::Advance(const Timer& timer)
{
    m_timer->Advance(timer);
}

bool TickTimer::Tick()
{
    // Convert tick seconds to tick units.
    TimeUnit tickUnits = Timer::ConvertToUnits((double)m_tickSeconds);

    // Do not allow forward tick counter to fall behind the previous tick time.
    // This allows timer with capped delta time to prevent a large number of ticks.
    m_forwardTickTimeUnits = std::max(m_timer->GetPreviousTimeUnits(), m_forwardTickTimeUnits);

    // Check if we should perform a tick.
    if(m_timer->GetCurrentTimeUnits() >= m_forwardTickTimeUnits)
    {
        // Move tick counter forward.
        m_forwardTickTimeUnits += tickUnits;

        // Track total time of all ticks.
        m_totalTickTimeUnits += tickUnits;

        // Save last successful tick time.
        m_lastTickSeconds = m_tickSeconds;

        // Signal successful tick.
        return true;
    }

    // Wait until we have enough time accumulated.
    // Return false to indicate that tick did not occur.
    return false;
}

float TickTimer::GetAlphaSeconds() const
{
    // Calculate accumulated time units since the last tick.
    TimeUnit accumulatedTickUnits = m_forwardTickTimeUnits - m_timer->GetCurrentTimeUnits();
    ASSERT(accumulatedTickUnits >= 0, "Accumulated tick units cannot be negative!");

    // Calculate normalized range between last two ticks.
    float accumulatedTickSeconds = (float)Timer::ConvertToSeconds(accumulatedTickUnits);
    float normalizedTickAlpha = (m_lastTickSeconds - accumulatedTickSeconds) / m_lastTickSeconds;
    ASSERT(normalizedTickAlpha >= 0.0f && normalizedTickAlpha <= 1.0f, "Tick alpha is not clamped in normal range!");

    // Return alpha time in normalized range between last two ticks.
    return normalizedTickAlpha;
}

float TickTimer::GetTickSeconds() const
{
    return m_tickSeconds;
}

double TickTimer::GetTotalTickSeconds() const
{
    return Timer::ConvertToSeconds(m_totalTickTimeUnits);
}

float TickTimer::GetLastTickSeconds() const
{
    return m_lastTickSeconds;
}
