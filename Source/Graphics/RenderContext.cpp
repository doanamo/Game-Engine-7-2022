/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Graphics/RenderContext.hpp"
#include <System/Window.hpp>
using namespace Graphics;

RenderContext::RenderContext() = default;
RenderContext::~RenderContext() = default;

RenderContext::CreateResult RenderContext::Create(const CreateParams& params)
{
    LOG("Creating rendering context...");
    LOG_SCOPED_INDENT();

    // Check arguments.
    CHECK_ARGUMENT_OR_RETURN(params.services != nullptr, Common::Failure(CreateErrors::InvalidArgument));

    // Create instance.
    auto instance = std::unique_ptr<RenderContext>(new RenderContext());

    // Acquire window service.
    instance->m_window = params.services->GetWindow();

    // Save initial render state.
    // Window has to set its OpenGL context as current for this to succeed.
    // Here we assume that at this point OpenGL context is in pristine state.
    // Maybe default render state should be collected immediately when context is created?
    instance->m_window->MakeContextCurrent();
    instance->m_currentState.Save();

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
