/*
    Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include "Core/System/EngineSystem.hpp"

/*
    Engine Metrics

    Utility subsystem for tracking important engine events and their timings, such as frame
    start and end times used to calculate framerate.
*/

namespace Core
{
    class EngineMetrics final : public EngineSystem
    {
        REFLECTION_ENABLE(EngineMetrics, EngineSystem)

    public:
        EngineMetrics();
        ~EngineMetrics() override;

        double GetFrameTimeLast() const
        {
            return m_frameTimeLast;
        }

        double GetFrameRateLast() const
        {
            return m_frameRateLast;
        }

        double GetFrameTimeAverage() const
        {
            return m_frameTimeAverage;
        }

        double GetFrameRateAverage() const
        {
            return m_frameRateAverage;
        }

    private:
        bool OnAttach(const EngineSystemStorage& engineSystems) override;
        void OnPreFrame() override;
        void OnPostFrame() override;

    private:
        double m_frameRateUpdateFrequency = 1.0;

        std::chrono::steady_clock::time_point m_frameStart;
        std::chrono::steady_clock::time_point m_frameEnd;
        std::chrono::steady_clock::time_point m_frameTimeUpdate;

        double m_frameTimeLast = 0.0;
        double m_frameRateLast = 0.0;
        double m_frameTimeAverage = 0.0;
        double m_frameRateAverage = 0.0;
        double m_frameTimeAccumulated = 0.0;
        int m_frameTimeAccumulations = 0;
    };
}

REFLECTION_TYPE(Core::EngineMetrics, Core::EngineSystem)
