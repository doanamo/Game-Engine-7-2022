/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Platform/Timer.hpp>

/*
    Tick Timer
*/

namespace Game
{
    class TickTimer final : public Common::NonCopyable
    {
    public:
        using TimeUnit = Platform::Timer::TimeUnit;
        using CreateResult = Common::Result<std::unique_ptr<TickTimer>, void>;
        static CreateResult Create();

    public:
        ~TickTimer();

        void Advance(const Platform::Timer& timer);
        bool Tick();
        void Reset();

        float CalculateAlphaSeconds() const;

        void SetTickSeconds(float tickTime)
        {
            m_tickSeconds = tickTime;
        }

        float GetTickSeconds() const
        {
            return m_tickSeconds;
        }

        float GetLastTickSeconds() const
        {
            return m_lastTickSeconds;
        }

        double GetTotalTickSeconds() const
        {
            return Platform::Timer::ConvertToSeconds(m_totalTickTimeUnits);
        }

    private:
        TickTimer();

    private:
        std::unique_ptr<Platform::Timer> m_timer;

        float m_tickSeconds = 1.0f / 10.0f;
        TimeUnit m_forwardTickTimeUnits = 0;
        TimeUnit m_totalTickTimeUnits = 0;
        float m_lastTickSeconds = 0.0f;
    };
}
