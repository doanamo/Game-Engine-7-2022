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
    class UpdateTimer : public System::Timer
    {
    public:
        UpdateTimer() = default;
        ~UpdateTimer() = default;

        UpdateTimer(UpdateTimer&& other);
        UpdateTimer& operator=(UpdateTimer&& other);

        bool Initialize() override;
        void Reset() override;
        bool Update(float updateTime);

        float GetAlphaTime() const;
        float GetLastUpdateTime() const;
        double GetTotalUpdateTime() const;

    private:
        uint64_t m_forwardUpdateCounter = 0;
        uint64_t m_totalUpdateCounter = 0;
        float m_lastUpdateTime = 0.0f;
    };
}
