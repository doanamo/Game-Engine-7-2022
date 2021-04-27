/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include "Core/Service.hpp"

/*
    Performance Metrics

    Utility subsystem for tracking important performance events and their
    timings, such as frame start/end times used to calculate framerate.
*/

namespace Core
{
    class PerformanceMetrics final : public Service
    {
        REFLECTION_ENABLE(PerformanceMetrics, Service)

    public:
        using CreateResult = Common::Result<std::unique_ptr<PerformanceMetrics>, void>;
        static CreateResult Create();

        ~PerformanceMetrics() override;

        void MarkFrameStart();
        void MarkFrameEnd();

        float GetFrameTime() const;
        float GetFrameRate() const;

    private:
        PerformanceMetrics();

        std::chrono::steady_clock::time_point m_frameStart;
        std::chrono::steady_clock::time_point m_frameEnd;
        std::chrono::steady_clock::time_point m_frameTimeUpdate;

        float m_frameTimeAverage = 0.0f;
        float m_frameTimeAccumulated = 0.0f;
        int m_frameTimeAccumulations = 0;
    };
}

REFLECTION_TYPE(Core::PerformanceMetrics, Core::Service)
