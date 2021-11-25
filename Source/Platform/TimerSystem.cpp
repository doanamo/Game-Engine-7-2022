/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Platform/TimerSystem.hpp"
#include "Platform/Timer.hpp"
#include <Core/System/SystemStorage.hpp>
#include <Core/Config/ConfigSystem.hpp>
using namespace Platform;

namespace
{
    const char* LogAttachFailed = "Failed to attach timer system! {}";
}

TimerSystem::TimerSystem()
    : m_timer(new Timer())
{
}

TimerSystem::~TimerSystem() = default;

void TimerSystem::OnRunEngine()
{
    m_timer->Reset();
}

bool TimerSystem::OnAttach(const Core::EngineSystemStorage& engineSystems)
{
    // Retrieve needed engine systems.
    auto& configSystem = engineSystems.Locate<Core::ConfigSystem>();

    // Read config variables.
    configSystem.Read(NAME("system.maxUpdateDelta"), &m_maxUpdateDelta);
    m_maxUpdateDelta = std::max(0.0f, m_maxUpdateDelta);

    return true;
}

void TimerSystem::OnBeginFrame()
{
    m_timer->Advance(m_maxUpdateDelta);
}
