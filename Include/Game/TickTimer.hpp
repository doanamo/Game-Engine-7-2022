/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <System/Timer.hpp>

/*
    Update Timer
*/

namespace Game
{
    class TickTimer final : public Common::NonCopyable
    {
    public:
        using TimeUnit = System::Timer::TimeUnit;
        using CreateResult = Common::Result<std::unique_ptr<TickTimer>, void>;
        static CreateResult Create();

    public:
        ~TickTimer();

        void Advance(const System::Timer& timer);
        bool Tick(float tickSeconds);
        void Reset();

        float GetAlphaSeconds() const;
        float GetLastTickSeconds() const;
        double GetTotalTickSeconds() const;

    private:
        TickTimer();

        std::unique_ptr<System::Timer> m_timer;
        TimeUnit m_forwardTickTimeUnits = 0;
        TimeUnit m_totalTickTimeUnits = 0;
        float m_lastTickSeconds = 0.0f;
    };
}
