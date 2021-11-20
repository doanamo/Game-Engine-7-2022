/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Platform/Precompiled.hpp"
#include "Platform/WindowSystem.hpp"
#include "Platform/Window.hpp"
#include <Core/SystemStorage.hpp>
#include <Core/ConfigSystem.hpp>
#include <Core/FrameRateLimiter.hpp>
using namespace Platform;

namespace
{
    const char* LogAttachFailed = "Failed to attach window system! {}";
}

WindowSystem::WindowSystem()
{
    m_receivers.focusChange.Bind<WindowSystem, &WindowSystem::OnWindowFocusChange>(this);
}

WindowSystem::~WindowSystem() = default;

bool WindowSystem::OnAttach(const Core::EngineSystemStorage& engineSystems)
{
    // Retrieve engine systems.
    auto& config = engineSystems.Locate<Core::ConfigSystem>();
    m_frameRateLimiter = &engineSystems.Locate<Core::FrameRateLimiter>();

    // Prepare window parameters.
    Window::CreateParams params;
    config.Read(NAME("window.title"), &params.title);
    config.Read(NAME("window.width"), &params.width);
    config.Read(NAME("window.height"), &params.height);
    config.Read(NAME("window.vsync"), &params.vsync);
    config.Read(NAME("window.visible"), &params.visible);
    config.Read(NAME("window.minWidth"), &params.minWidth);
    config.Read(NAME("window.minHeight"), &params.minHeight);
    config.Read(NAME("window.maxWidth"), &params.maxWidth);
    config.Read(NAME("window.maxHeight"), &params.maxHeight);

    // Create window instance.
    m_window = Window::Create(params).UnwrapOr(nullptr);
    if(m_window == nullptr)
    {
        LOG_ERROR(LogAttachFailed, "Could not create window.");
        return false;
    }

    // Subscribe to window events.
    if(!m_window->events.Subscribe(m_receivers.focusChange))
    {
        LOG_ERROR(LogAttachFailed, "Could not subscribe to window events.");
        return false;
    }

    return true;
}

void WindowSystem::OnRunEngine()
{
    m_window->MakeContextCurrent();
}

void WindowSystem::OnBeginFrame()
{
    m_window->ProcessEvents();
}

void WindowSystem::OnEndFrame()
{
    m_window->Present();
}

bool WindowSystem::IsRequestingExit()
{
    if(m_window->ShouldClose())
    {
        LOG_INFO("Requesting exit because window received close event.");
        return true;
    }

    return false;
}

void WindowSystem::OnWindowFocusChange(const WindowEvents::Focus& event)
{
    LOG_TRACE("Window focus state changed to {}.", event.focused ? "foreground" : "background");
    m_frameRateLimiter->ToggleWindowFocusState(event.focused);
}
