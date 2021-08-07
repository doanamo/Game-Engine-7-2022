
/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "System/Precompiled.hpp"
#include "System/Window.hpp"
#include <Core/SystemStorage.hpp>
#include <Core/ConfigSystem.hpp>
using namespace System;

namespace
{
    const char* LogAttachFailed = "Failed to attach window! {}";
}

Window::Window()
    : m_context(*this)
{
    events.Register<void, WindowEvents::Move>();
    events.Register<void, WindowEvents::Resize>();
    events.Register<void, WindowEvents::Resize>();
    events.Register<void, WindowEvents::Focus>();
    events.Register<void, WindowEvents::Close>();
    events.Finalize();
}

Window::~Window()
{
    // We need to destroy handle at end of lifetime instead of system detach,
    // as some resources may still depend on e.g. OpenGL context to be present.
    if(m_context.handle)
    {
        glfwDestroyWindow(m_context.handle);
        m_context.handle = nullptr;
    }
}

bool Window::OnAttach(const Core::EngineSystemStorage& engineSystems)
{
    // Retrieve config variables.
    auto* config = engineSystems.Locate<Core::ConfigSystem>();
    if(config == nullptr)
    {
        LOG_ERROR(LogAttachFailed, "Could not locate config.");
        return false;
    }

    m_title = config->Get<std::string>(NAME_CONSTEXPR("window.title")).UnwrapOr("Game");
    int width = config->Get<int>(NAME_CONSTEXPR("window.width")).UnwrapOr(1024);
    int height = config->Get<int>(NAME_CONSTEXPR("window.height")).UnwrapOr(576);
    bool vsync = config->Get<bool>(NAME_CONSTEXPR("window.vsync")).UnwrapOr(true);
    bool visible = config->Get<bool>(NAME_CONSTEXPR("window.visible")).UnwrapOr(true);
    int minWidth = config->Get<int>(NAME_CONSTEXPR("window.minWidth")).UnwrapOr(-1);
    int minHeight = config->Get<int>(NAME_CONSTEXPR("window.minHeight")).UnwrapOr(-1);
    int maxWidth = config->Get<int>(NAME_CONSTEXPR("window.maxWidth")).UnwrapOr(-1);
    int maxHeight = config->Get<int>(NAME_CONSTEXPR("window.maxHeight")).UnwrapOr(-1);

    width = std::max(0, width);
    height = std::max(0, height);

    // Setup window hints.
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);

#ifdef __EMSCRIPTEN__
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#else
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    glfwWindowHint(GLFW_VISIBLE, visible ? 1 : 0);

    // Create window.
    m_context.handle = glfwCreateWindow(width, height, m_title.c_str(), nullptr, nullptr);
    if(m_context.handle == nullptr)
    {
        LOG_ERROR(LogAttachFailed, "Could not create window.");
        return false;
    }

    glfwSetWindowUserPointer(m_context.handle, &m_context);
    glfwSetWindowSizeLimits(m_context.handle, minWidth, minHeight, maxWidth, maxHeight);
    glfwSetWindowPosCallback(m_context.handle, Window::MoveCallback);
    glfwSetFramebufferSizeCallback(m_context.handle, Window::ResizeCallback);
    glfwSetWindowFocusCallback(m_context.handle, Window::FocusCallback);
    glfwSetWindowCloseCallback(m_context.handle, Window::CloseCallback);

    glfwMakeContextCurrent(m_context.handle);
    glfwSwapInterval((int)vsync);

    // Prepare OpenGL function and extension loader.
    if(!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress))
    {
        LOG_ERROR(LogAttachFailed, "Could not load OpenGL function and extension loader.");
        return false;
    }

    ASSERT(glGetError() == GL_NO_ERROR, "OpenGL error occurred during context initialization!");

    // Retrieve window and context info.
    glfwGetFramebufferSize(m_context.handle, &m_width, &m_height);
    LOG_INFO("Resolution is {}x{}.", m_width, m_height);

    int glInterface = glfwGetWindowAttrib(m_context.handle, GLFW_CLIENT_API);
    int glMajor = glfwGetWindowAttrib(m_context.handle, GLFW_CONTEXT_VERSION_MAJOR);
    int glMinor = glfwGetWindowAttrib(m_context.handle, GLFW_CONTEXT_VERSION_MINOR);
    LOG_INFO("Using OpenGL {}{}.{} context.",
        glInterface == GLFW_OPENGL_API ? "" : "ES ", glMajor, glMinor);

    return true;
}

void Window::OnBeginFrame()
{
    ProcessEvents();
}

void Window::OnEndFrame()
{
    Present();
}

void Window::MakeContextCurrent()
{
    ASSERT(m_context.handle);
    glfwMakeContextCurrent(m_context.handle);
}

void Window::ProcessEvents()
{
    // Poll window events.
    ASSERT(m_context.handle);
    glfwPollEvents();

    // Save new window size.
    if(m_sizeChanged)
    {
        glfwGetFramebufferSize(m_context.handle, &m_width, &m_height);
        LOG_INFO("Resolution changed to {}x{}.", m_width, m_height);
        m_sizeChanged = false;
    }
}

void Window::Present()
{
    ASSERT(m_context.handle);
    glfwSwapBuffers(m_context.handle);

    GLenum error;
    while((error = glGetError()) != GL_NO_ERROR)
    {
        LOG_WARNING("Found uncaught OpenGL error in the last frame (code {:#06x})!", error);
    }

    ASSERT(error == GL_NO_ERROR, "Uncaught OpenGL error(s) encountered!");
}

void Window::Close()
{
    ASSERT(m_context.handle);
    glfwSetWindowShouldClose(m_context.handle, GL_TRUE);
}

void Window::SetTitle(std::string title)
{
    ASSERT(m_context.handle);
    glfwSetWindowTitle(m_context.handle, title.c_str());
    m_title = title;
}

void Window::SetVisibility(bool show)
{
    ASSERT(m_context.handle);

    if(show)
    {
        glfwShowWindow(m_context.handle);
    }
    else
    {
        glfwHideWindow(m_context.handle);
    }
}

bool Window::ShouldClose() const
{
    ASSERT(m_context.handle);
    return glfwWindowShouldClose(m_context.handle) == 0;
}

bool Window::IsFocused() const
{
    ASSERT(m_context.handle);
    return glfwGetWindowAttrib(m_context.handle, GLFW_FOCUSED) > 0;
}

Window& Window::GetWindowFromUserData(GLFWwindow* handle)
{
    ASSERT(handle != nullptr, "Window handle is invalid!");
    WindowContext* context = reinterpret_cast<WindowContext*>(glfwGetWindowUserPointer(handle));
    ASSERT(context != nullptr, "Window context is null!");

    return context->window;
}

void Window::MoveCallback(GLFWwindow* handle, int x, int y)
{
    WindowEvents::Move eventData;
    eventData.x = x;
    eventData.y = y;

    Window& window = GetWindowFromUserData(handle);
    window.events.Dispatch<void>(eventData);
}

void Window::ResizeCallback(GLFWwindow* handle, int width, int height)
{
    WindowEvents::Resize eventData;
    eventData.width = width;
    eventData.height = height;

    Window& window = GetWindowFromUserData(handle);
    window.events.Dispatch<void>(eventData).Unwrap();
    window.m_sizeChanged = true;
}

void Window::FocusCallback(GLFWwindow* handle, int focused)
{
    WindowEvents::Focus eventData;
    eventData.focused = focused > 0;

    Window& window = GetWindowFromUserData(handle);
    window.events.Dispatch<void>(eventData).Unwrap();
}

void Window::CloseCallback(GLFWwindow* handle)
{
    Window& window = GetWindowFromUserData(handle);
    window.events.Dispatch<void>(WindowEvents::Close{}).Unwrap();
}
