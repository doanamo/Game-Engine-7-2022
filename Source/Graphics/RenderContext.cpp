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
    // Make selected window's OpenGL context current.
    window->MakeContextCurrent();

    // We need the initial state.
    m_states.push_back(RenderState());

    SCOPE_GUARD_IF(!m_initialized, Utility::ClearContainer(m_states));

    // Save window reference.
    m_window = window;

    // Success!
    LOG_INFO() << "Success!";

    return m_initialized = true;
}

void RenderContext::MakeCurrent()
{
    VERIFY(m_window != nullptr, "Window instance is null!");

    m_window->MakeContextCurrent();
}

RenderState& RenderContext::GetState()
{
    VERIFY(!m_states.empty(), "Array of render states is empty!");

    return m_states.back();
}

bool RenderContext::IsValid() const
{
    return m_initialized;
}
