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
    auto* configSystem = engineSystems.Locate<ConfigSystem>();
    if(configSystem == nullptr)
    {
        LOG_ERROR("Failed to attach engine metrics! Could not retrieve window.");
        return false;
    }

    m_frameRateUpdateFrequency =
        configSystem->Get<double>(NAME_CONSTEXPR("metrics.frameRateUpdateFrequency"))
        .UnwrapOr(m_frameRateUpdateFrequency);

    return true;
}

void EngineMetrics::OnBeginFrame()
{
    m_frameStart = std::chrono::steady_clock::now();
}

void EngineMetrics::OnEndFrame()
{
    m_frameEnd = std::chrono::steady_clock::now();

    // Count accumulated frames between frame rate counter update.
    m_frameTimeAccumulated += std::chrono::duration<double>(m_frameEnd - m_frameStart).count();
    m_frameTimeAccumulations += 1;

    // Calculate frame rate on frame end.
    double timeElapsed = std::chrono::duration<double>(m_frameEnd - m_frameTimeUpdate).count();
    if(timeElapsed >= m_frameRateUpdateFrequency)
    {
        m_frameTimeUpdate = m_frameEnd;
        m_frameTimeAverage = m_frameTimeAccumulated / static_cast<double>(m_frameTimeAccumulations);
        m_frameTimeAccumulated = 0.0;
        m_frameTimeAccumulations = 0;
    }
}

float EngineMetrics::GetFrameTime() const
{
    return static_cast<float>(m_frameTimeAverage);
}

float EngineMetrics::GetFrameRate() const
{
    return static_cast<float>(1.0 / m_frameTimeAverage);
}
