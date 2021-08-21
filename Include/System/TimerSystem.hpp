/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Core/EngineSystem.hpp>

/*
    Timer System

    Wrapper for timer to be used as engine system.
*/

namespace System
{
    class Timer;

    class TimerSystem final : public Core::EngineSystem
    {
        REFLECTION_ENABLE(TimerSystem, Core::EngineSystem)

    public:
        TimerSystem();
        ~TimerSystem() override;

        const Timer& GetTimer() const
        {
            ASSERT(m_timer);
            return *m_timer;
        }

    private:
        bool OnAttach(const Core::EngineSystemStorage& engineSystems) override;
        void OnRunEngine() override;
        void OnBeginFrame() override;

    private:
        std::unique_ptr<Timer> m_timer;
        float m_maxUpdateDelta = 1.0f;
    };
}

REFLECTION_TYPE(System::TimerSystem, Core::EngineSystem)
