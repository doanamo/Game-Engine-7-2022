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
    class UpdateTimer final : public Common::NonCopyable
    {
    public:
        using TimeUnit = System::Timer::TimeUnit;

        using CreateResult = Common::Result<std::unique_ptr<UpdateTimer>, void>;
        static CreateResult Create();

    public:
        ~UpdateTimer();

        void Advance(const System::Timer& timer);
        bool Update(float updateSeconds);
        void Reset();

        float GetAlphaSeconds() const;
        float GetLastUpdateSeconds() const;
        double GetTotalUpdateSeconds() const;

    private:
        UpdateTimer();

    private:
        std::unique_ptr<System::Timer> m_timer;
        TimeUnit m_forwardUpdateTimeUnits = 0;
        TimeUnit m_totalUpdateTimeUnits = 0;
        float m_lastUpdateSeconds = 0.0f;
    };
}
