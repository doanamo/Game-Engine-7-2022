/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include "Core/System/EngineSystem.hpp"

/*
    Framerate Limiter
*/

namespace Core
{
    class EngineMetrics;

    class FrameRateLimiter final : public EngineSystem
    {
        REFLECTION_ENABLE(FrameRateLimiter, EngineSystem)

    public:
        FrameRateLimiter();
        ~FrameRateLimiter() override;

        void ToggleWindowFocusState(bool state)
        {
            m_windowFocusState = state;
        }

    private:
        bool OnAttach(const EngineSystemStorage& engineSystems) override;
        void OnPostFrame() override;

    private:
        EngineMetrics* m_engineMetrics = nullptr;
        bool m_windowFocusState = true;
        float m_foregroundFpsLimit = 0.0f;
        float m_backgroundFpsLimit = 10.0f;
        double m_lastSleepError = 0.0;
    };
}

REFLECTION_TYPE(Core::FrameRateLimiter, Core::EngineSystem)
