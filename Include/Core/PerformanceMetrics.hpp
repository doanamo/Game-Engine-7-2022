/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    Performance Metrics
*/

namespace Core
{
    class PerformanceMetrics final : Common::NonCopyable
    {
    public:
        using CreateResult = Common::Result<std::unique_ptr<PerformanceMetrics>, void>;
        static CreateResult Create();

    public:
        ~PerformanceMetrics();

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
