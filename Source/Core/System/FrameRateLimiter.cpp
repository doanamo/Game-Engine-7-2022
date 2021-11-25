/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Core/System/FrameRateLimiter.hpp"
#include "Core/System/SystemStorage.hpp"
#include "Core/System/EngineMetrics.hpp"
#include "Core/Config/ConfigSystem.hpp"
using namespace Core;

namespace
{
    const char* LogAttachFailed = "Failed to attach frame rate limiter! {}";
}

FrameRateLimiter::FrameRateLimiter() = default;
FrameRateLimiter::~FrameRateLimiter() = default;

bool FrameRateLimiter::OnAttach(const SystemStorage<EngineSystem>& engineSystems)
{
    // Retrieve engine systems.
    m_engineMetrics = &engineSystems.Locate<Core::EngineMetrics>();

    // Read config variables.
    auto& config = engineSystems.Locate<Core::ConfigSystem>();
    config.Read(NAME("core.foregroundFpsLimit"), &m_foregroundFpsLimit);
    config.Read(NAME("core.backgroundFpsLimit"), &m_backgroundFpsLimit);

    return true;
}

void FrameRateLimiter::OnPostFrame()
{
    float fpsLimit = m_windowFocusState ? m_foregroundFpsLimit : m_backgroundFpsLimit;

    if(fpsLimit > 0.0f)
    {
        // Calculate sleep time needed to reach target frame time.
        double frameTimeLimit = 1.0 / fpsLimit;
        double sleepTime = frameTimeLimit - m_engineMetrics->GetFrameTimeLast() + m_lastSleepError;

        // Sleep the thread if needed until we reach target frame time.
        auto sleepStart = std::chrono::steady_clock::now();

        if(sleepTime > 0.0)
        {
            std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
        }
        
        // Calculate sleep time error and factor it next time we sleep the thread.
        m_lastSleepError = sleepTime - std::chrono::duration<double>(
            std::chrono::steady_clock::now() - sleepStart).count();
    }
}
