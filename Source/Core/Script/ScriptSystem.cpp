/*
    Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
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
    // Create Lua script state.
    m_state = ScriptState::Create().UnwrapOr(nullptr);
    if(m_state == nullptr)
    {
        LOG_ERROR(LogAttachFailed, "Could not create script state.");
        return false;
    }

    // Bind script interface.
    if(!BindScriptInterface(*m_state))
    {
        LOG_ERROR(LogAttachFailed, "Could not bind script interface.");
        return false;
    }

    // Make sure that we did not leave anything on the stack.
    ASSERT(lua_gettop(*m_state) == 0, "Lua stack is not empty!");

    return true;
}

void ScriptSystem::OnEndFrame()
{
    m_state->CollectGarbage();
}
