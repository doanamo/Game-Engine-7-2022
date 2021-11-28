/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Core/Script/ScriptSystem.hpp"
using namespace Core;

namespace
{
    const char* LogAttachFailed = "Failed to attach script system! {}";
}

ScriptSystem::ScriptSystem() = default;
ScriptSystem::~ScriptSystem() = default;

bool ScriptSystem::OnAttach(const EngineSystemStorage& systemStorage)
{
    m_state = ScriptState::Create().UnwrapOr(nullptr);
    if(m_state == nullptr)
    {
        LOG_ERROR(LogAttachFailed, "Could not create script state.");
        return false;
    }

    return true;
}

void ScriptSystem::OnEndFrame()
{
    m_state->CollectGarbage();
}
