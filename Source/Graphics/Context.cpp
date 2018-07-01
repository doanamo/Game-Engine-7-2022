/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Graphics/Context.hpp"
#include "System/Window.hpp"
using namespace Graphics;

Context::Context() :
    m_window(nullptr),
    m_initialized(false)
{
}

Context::~Context()
{
}

bool Context::Initialize(System::Window* window)
{
    // Make selected window's OpenGL context current.
    window->MakeContextCurrent();

    // We need the initial state.
    m_states.push_back(State());

    SCOPE_GUARD_IF(!m_initialized, Utility::ClearContainer(m_states));

    // Save window reference.
    m_window = window;

    // Success!
    LOG_INFO() << "Success!";

    return m_initialized = true;
}

void Context::MakeCurrent()
{
    VERIFY(m_window != nullptr, "Window instance is null!");

    m_window->MakeContextCurrent();
}

const State& Context::GetState() const
{
    VERIFY(!m_states.empty(), "Array of render states is empty!");

    return m_states.back();
}

bool Context::IsValid() const
{
    return m_initialized;
}
