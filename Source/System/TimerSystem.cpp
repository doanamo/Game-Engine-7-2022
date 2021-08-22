/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "System/Precompiled.hpp"
#include "System/TimerSystem.hpp"
#include "System/Timer.hpp"
#include <Core/SystemStorage.hpp>
#include <Core/ConfigSystem.hpp>
using namespace System;

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
    float maxUpdateDelta = configSystem.Get<float>(
        NAME_CONSTEXPR("system.maxUpdateDelta"))
        .UnwrapOr(m_maxUpdateDelta);

    m_maxUpdateDelta = std::max(0.0f, m_maxUpdateDelta);

    return true;
}

void TimerSystem::OnBeginFrame()
{
    m_timer->Advance(m_maxUpdateDelta);
}
