/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Graphics/Precompiled.hpp"
#include "Graphics/RenderContext.hpp"
#include <Core/SystemStorage.hpp>
#include <System/WindowSystem.hpp>
#include <System/Window.hpp>
using namespace Graphics;

namespace
{
    const char* LogAttachFailed = "Failed to attach render context! {}";
}

RenderContext::RenderContext() = default;
RenderContext::~RenderContext() = default;

bool RenderContext::OnAttach(const Core::EngineSystemStorage& engineSystems)
{
    // Retrieve needed engine systems.
    m_windowSystem = engineSystems.Locate<System::WindowSystem>();
    if(!m_windowSystem)
    {
        LOG_ERROR(LogAttachFailed, "Could not locate window system.");
        return false;
    }

    // Save initial render state. Window has to set its OpenGL context as current for this to
    // succeed. Here we assume that at this point OpenGL context is still in pristine state, but
    // maybe default render state should be collected immediately when context is created?
    m_windowSystem->GetWindow().MakeContextCurrent();
    m_currentState.Save();

    return true;
}

void RenderContext::MakeCurrent()
{
    m_windowSystem->GetWindow().MakeContextCurrent();
}

RenderState& RenderContext::PushState()
{
    // Push copy of current state.
    m_pushedStates.push(m_currentState);
    return m_currentState;
}

void RenderContext::PopState()
{
    ASSERT(!m_pushedStates.empty(), "Trying to pop non existing render state!");

    // Discard current state and apply last pushed state.
    m_currentState.Apply(m_pushedStates.top());
    m_pushedStates.pop();
}
