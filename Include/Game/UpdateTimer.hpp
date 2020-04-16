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
    class UpdateTimer final : public Resettable<UpdateTimer>
    {
    public:
        UpdateTimer();
        ~UpdateTimer();

        bool Initialize();
        void Tick(const System::Timer& timer);
        bool Update(float updateTime);
        void Reset();

        float GetAlphaTime() const;
        float GetLastUpdateTime() const;
        double GetTotalUpdateTime() const;

    private:
        System::Timer m_timer;
        uint64_t m_forwardUpdateCounter = 0;
        uint64_t m_totalUpdateCounter = 0;
        float m_lastUpdateTime = 0.0f;
    };
}
