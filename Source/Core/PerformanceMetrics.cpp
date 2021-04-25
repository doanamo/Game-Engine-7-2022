/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Core/Precompiled.hpp"
#include "Core/PerformanceMetrics.hpp"
using namespace Core;

namespace
{
    float FrameRateUpdateFrequency = 1.0f;
}

PerformanceMetrics::PerformanceMetrics() = default;
PerformanceMetrics::~PerformanceMetrics() = default;

PerformanceMetrics::CreateResult PerformanceMetrics::Create()
{
    auto instance = std::unique_ptr<PerformanceMetrics>(new PerformanceMetrics());

    LOG_SUCCESS("Created performance metrics instance.");
    return Common::Success(std::move(instance));
}

void PerformanceMetrics::MarkFrameStart()
{
    m_frameStart = std::chrono::steady_clock::now();
}

void PerformanceMetrics::MarkFrameEnd()
{
    m_frameEnd = std::chrono::steady_clock::now();

    m_frameTimeAccumulated += std::chrono::duration<float>(m_frameEnd - m_frameStart).count();
    m_frameTimeAccumulations += 1;

    float timeElapsed = std::chrono::duration<float>(m_frameEnd - m_frameTimeUpdate).count();
    if(timeElapsed >= FrameRateUpdateFrequency)
    {
        m_frameTimeUpdate = m_frameEnd;
        m_frameTimeAverage = m_frameTimeAccumulated / static_cast<float>(m_frameTimeAccumulations);
        m_frameTimeAccumulated = 0;
        m_frameTimeAccumulations = 0;
    }
}

float PerformanceMetrics::GetFrameTime() const
{
    return m_frameTimeAverage;
}

float PerformanceMetrics::GetFrameRate() const
{
    return 1.0f / GetFrameTime();
}
