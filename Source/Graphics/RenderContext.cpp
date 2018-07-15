/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Graphics/RenderContext.hpp"
#include "System/Window.hpp"
using namespace Graphics;

RenderContext::RenderContext() :
    m_window(nullptr),
    m_initialized(false)
{
}

RenderContext::~RenderContext()
{
}

bool RenderContext::Initialize(System::Window* window)
{
    LOG() << "Initializing rendering context..." << LOG_INDENT();

    // Make sure that the instance is not initialized.
    VERIFY(!m_initialized, "Render context has already been initialized!");

    // Make selected window's OpenGL context current.
    window->MakeContextCurrent();

    // Initialize the initial state.
    if(!m_currentState.Initialize())
        return false;

    // Save window reference.
    m_window = window;

    // Success!
    return m_initialized = true;
}

void RenderContext::MakeCurrent()
{
    VERIFY(m_initialized, "Render context is not initialized!");
    VERIFY(m_window != nullptr, "Window instance is null!");

    m_window->MakeContextCurrent();
}

void RenderContext::PushState()
{
    VERIFY(m_initialized, "Render context is not initialized!");

    // Push a copy of the current state.
    m_pushedStates.emplace(m_currentState);
}

void RenderContext::PopState()
{
    VERIFY(m_initialized, "Render context is not initialized!");
    VERIFY(!m_pushedStates.empty(), "Trying to pop a non existing render state!");

    // Apply changes from the stack below.
    m_currentState.Apply(m_pushedStates.top());

    // Remove the applied state.
    m_pushedStates.pop();
}

RenderState& RenderContext::GetState()
{
    VERIFY(m_initialized, "Render context is not initialized!");

    return m_currentState;
}

bool RenderContext::IsValid() const
{
    return m_initialized;
}
