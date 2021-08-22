/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Core/Precompiled.hpp"
#include "Core/EngineMetrics.hpp"
#include "Core/SystemStorage.hpp"
#include "Core/ConfigSystem.hpp"
using namespace Core;

EngineMetrics::EngineMetrics() = default;
EngineMetrics::~EngineMetrics() = default;

bool EngineMetrics::OnAttach(const SystemStorage<EngineSystem>& engineSystems)
{
    // Retrieve needed engine systems
    auto* configSystem = engineSystems.Locate<ConfigSystem>();
    if(configSystem == nullptr)
    {
        LOG_ERROR("Failed to attach engine metrics! Could not retrieve window.");
        return false;
    }

    // Read config variables.
    m_frameRateUpdateFrequency =
        configSystem->Get<double>(NAME_CONSTEXPR("metrics.frameRateUpdateFrequency"))
        .UnwrapOr(m_frameRateUpdateFrequency);

    // Reset time points.
    m_frameStart = std::chrono::steady_clock::now();
    m_frameEnd = std::chrono::steady_clock::now();
    m_frameTimeUpdate = std::chrono::steady_clock::now();

    return true;
}

void EngineMetrics::OnPreFrame()
{
    m_frameStart = std::chrono::steady_clock::now();
}

void EngineMetrics::OnPostFrame()
{
    // Calculate current frame time from time slice without any frame rate limiting.
    // For last frame rate we only measure time between pre and post frame calls.
    auto timeNow = std::chrono::steady_clock::now();
    m_frameTimeLast = std::chrono::duration<double>(timeNow - m_frameStart).count();
    m_frameRateLast = 1.0 / m_frameTimeLast;

    // Count time slice for average calculation with frame rate limiting applied.
    // For average frame rate we measure entire time since previous post frame call.
    m_frameStart = m_frameEnd;
    m_frameEnd = timeNow;

    // Count accumulated frames between frame rate counter update.
    m_frameTimeAccumulated += std::chrono::duration<double>(m_frameEnd - m_frameStart).count();
    m_frameTimeAccumulations += 1;

    // Calculate frame rate on frame end.
    double timeElapsed = std::chrono::duration<double>(m_frameEnd - m_frameTimeUpdate).count();
    if(timeElapsed >= m_frameRateUpdateFrequency)
    {
        m_frameTimeUpdate = m_frameEnd;
        m_frameTimeAverage = m_frameTimeAccumulated / static_cast<double>(m_frameTimeAccumulations);
        m_frameRateAverage = 1.0 / m_frameTimeAverage;
        m_frameTimeAccumulated = 0.0;
        m_frameTimeAccumulations = 0;
    }
}
