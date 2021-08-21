/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "System/Precompiled.hpp"
#include "System/WindowSystem.hpp"
#include "System/Window.hpp"
#include <Core/SystemStorage.hpp>
#include <Core/ConfigSystem.hpp>
using namespace System;

namespace
{
    const char* LogAttachFailed = "Failed to attach window system! {}";
}

WindowSystem::WindowSystem() = default;
WindowSystem::~WindowSystem() = default;

bool WindowSystem::OnAttach(const Core::EngineSystemStorage& engineSystems)
{
    // Retrieve config variables.
    auto* config = engineSystems.Locate<Core::ConfigSystem>();
    if(config == nullptr)
    {
        LOG_ERROR(LogAttachFailed, "Could not locate config.");
        return false;
    }

    // Prepare window parameters.
    Window::CreateParams params;

    params.title = config->Get<std::string>(NAME_CONSTEXPR("window.title")).UnwrapOr(params.title);
    params.width = config->Get<int>(NAME_CONSTEXPR("window.width")).UnwrapOr(params.width);
    params.height = config->Get<int>(NAME_CONSTEXPR("window.height")).UnwrapOr(params.height);
    params.vsync = config->Get<bool>(NAME_CONSTEXPR("window.vsync")).UnwrapOr(params.vsync);
    params.visible = config->Get<bool>(NAME_CONSTEXPR("window.visible")).UnwrapOr(params.visible);
    params.minWidth = config->Get<int>(NAME_CONSTEXPR("window.minWidth")).UnwrapOr(params.minWidth);
    params.minHeight = config->Get<int>(NAME_CONSTEXPR("window.minHeight")).UnwrapOr(params.minHeight);
    params.maxWidth = config->Get<int>(NAME_CONSTEXPR("window.maxWidth")).UnwrapOr(params.maxWidth);
    params.maxHeight = config->Get<int>(NAME_CONSTEXPR("window.maxHeight")).UnwrapOr(params.maxHeight);

    // Create window instance.
    m_window = Window::Create(params).UnwrapOr(nullptr);
    if(m_window == nullptr)
    {
        LOG_ERROR(LogAttachFailed, "Could not create window.");
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
