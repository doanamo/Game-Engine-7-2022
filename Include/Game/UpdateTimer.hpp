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
        using CreateResult = Common::Result<std::unique_ptr<UpdateTimer>, void>;
        static CreateResult Create();

    public:
        ~UpdateTimer();

        void Tick(const System::Timer& timer);
        bool Update(float updateTime);
        void Reset();

        float GetAlphaTime() const;
        float GetLastUpdateTime() const;
        double GetTotalUpdateTime() const;

    private:
        UpdateTimer();

    private:
        std::unique_ptr<System::Timer> m_timer;
        uint64_t m_forwardUpdateCounter = 0;
        uint64_t m_totalUpdateCounter = 0;
        float m_lastUpdateTime = 0.0f;
    };
}
