/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Graphics/RenderContext.hpp"
#include <System/Window.hpp>
using namespace Graphics;

RenderContext::RenderContext() = default;
RenderContext::~RenderContext() = default;

RenderContext::InitializeResult RenderContext::Initialize(System::Window* window)
{
    LOG("Initializing rendering context...");
    LOG_SCOPED_INDENT();

    // Setup initialization guard.
    VERIFY(!m_initialized, "Instance has already been initialized!");
    SCOPE_GUARD_IF(!m_initialized, this->Reset());

    // Check arguments.
    CHECK_ARGUMENT_OR_RETURN(window != nullptr, Failure(InitializeErrors::InvalidArgument));

    // Make window context current.
    window->MakeContextCurrent();

    // Initialize initial state.
    if(!m_currentState.Initialize())
    {
        LOG_ERROR("Failed to initialize render state!");
        return Failure(InitializeErrors::FailedStateInitialization);
    }

    // Save window reference.
    m_window = window;

    // Success!
    m_initialized = true;
    return Success();
}

void RenderContext::MakeCurrent()
{
    ASSERT(m_initialized, "Render context has not been initialized!");
    m_window->MakeContextCurrent();
}

RenderState& RenderContext::PushState()
{
    ASSERT(m_initialized, "Render context has not been initialized!");

    // Push copy of the current state.
    m_pushedStates.emplace(m_currentState);

    // Return current state for convenience.
    return m_currentState;
}

void RenderContext::PopState()
{
    ASSERT(m_initialized, "Render context has not been initialized!");
    ASSERT(!m_pushedStates.empty(), "Trying to pop non existing render state!");

    // Apply changes from the stack below.
    m_currentState.Apply(m_pushedStates.top());

    // Remove applied state.
    m_pushedStates.pop();
}

RenderState& RenderContext::GetState()
{
    ASSERT(m_initialized, "Render context has not been initialized!");
    return m_currentState;
}

bool RenderContext::IsInitialized() const
{
    return m_initialized;
}
