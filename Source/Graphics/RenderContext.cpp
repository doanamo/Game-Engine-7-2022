/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Graphics/RenderContext.hpp"
#include <System/Window.hpp>
using namespace Graphics;

RenderContext::RenderContext() = default;
RenderContext::~RenderContext() = default;

RenderContext::CreateResult RenderContext::Create(System::Window* window)
{
    LOG("Creating rendering context...");
    LOG_SCOPED_INDENT();

    // Check arguments.
    CHECK_ARGUMENT_OR_RETURN(window != nullptr, Common::Failure(CreateErrors::InvalidArgument));

    // Create instance.
    auto instance = std::unique_ptr<RenderContext>(new RenderContext());

    // Make window context current.
    window->MakeContextCurrent();

    // Save initial render state.
    instance->m_currentState.Save();

    // Save window reference.
    instance->m_window = window;

    // Success!
    return Common::Success(std::move(instance));
}

void RenderContext::MakeCurrent()
{
    m_window->MakeContextCurrent();
}

RenderState& RenderContext::PushState()
{
    // Push copy of current state.
    m_pushedStates.push(m_currentState);

    // Return current state for convenience.
    return m_currentState;
}

RenderState& RenderContext::GetState()
{
    return m_currentState;
}

void RenderContext::PopState()
{
    ASSERT(!m_pushedStates.empty(), "Trying to pop non existing render state!");

    // Apply changes from the stack below.
    m_currentState.Apply(m_pushedStates.top());

    // Remove applied state.
    m_pushedStates.pop();
}
