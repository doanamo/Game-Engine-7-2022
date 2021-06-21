/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "System/Precompiled.hpp"
#include "System/Window.hpp"
#include <Core/ServiceStorage.hpp>
#include <Core/Config.hpp>
using namespace System;

Window::Window() :
    m_context(*this)
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
    if(m_context.handle)
    {
        glfwDestroyWindow(m_context.handle);
        m_context.handle = nullptr;
    }
}

bool Window::OnAttach(const Core::ServiceStorage* services)
{
    LOG("Creating window...");
    LOG_SCOPED_INDENT();

    Core::Config* config = services->Locate<Core::Config>();

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

    m_context.handle = glfwCreateWindow(width, height, m_title.c_str(), nullptr, nullptr);
    if(m_context.handle == nullptr)
    {
        LOG_ERROR("Could not create GLFW window!");
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

    if(!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress))
    {
        LOG_ERROR("Could not load OpenGL ES extensions!");
        return false;
    }

    ASSERT(glGetError() == GL_NO_ERROR, "OpenGL error occurred during context initialization!");

    int windowWidth, windowHeight;
    glfwGetFramebufferSize(m_context.handle, &windowWidth, &windowHeight);
    LOG_INFO("Resolution is {}x{}.", windowWidth, windowHeight);

    int glInterface = glfwGetWindowAttrib(m_context.handle, GLFW_CLIENT_API);
    int glMajor = glfwGetWindowAttrib(m_context.handle, GLFW_CONTEXT_VERSION_MAJOR);
    int glMinor = glfwGetWindowAttrib(m_context.handle, GLFW_CONTEXT_VERSION_MINOR);
    LOG_INFO("Using OpenGL {}{}.{} context.",
        glInterface == GLFW_OPENGL_API ? "" : "ES ", glMajor, glMinor);

    return true;
}

void Window::MakeContextCurrent()
{
    ASSERT(m_context.handle);
    glfwMakeContextCurrent(m_context.handle);
}

void Window::ProcessEvents()
{
    ASSERT(m_context.handle);
    glfwPollEvents();

    if(m_sizeChanged)
    {
        int windowWidth, windowHeight;
        glfwGetFramebufferSize(m_context.handle, &windowWidth, &windowHeight);
        LOG_INFO("Resolution changed to {}x{}.", windowWidth, windowHeight);

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

std::string Window::GetTitle() const
{
    return m_title;
}

int Window::GetWidth() const
{
    int width = 0;
    ASSERT(m_context.handle);
    glfwGetFramebufferSize(m_context.handle, &width, nullptr);
    return width;
}

int Window::GetHeight() const
{
    int height = 0;
    ASSERT(m_context.handle);
    glfwGetFramebufferSize(m_context.handle, nullptr, &height);
    return height;
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

WindowContext& Window::GetContext()
{
    return m_context;
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
    Window& window = GetWindowFromUserData(handle);

    WindowEvents::Move eventData;
    eventData.x = x;
    eventData.y = y;

    window.events.Dispatch<void>(eventData);
}

void Window::ResizeCallback(GLFWwindow* handle, int width, int height)
{
    Window& window = GetWindowFromUserData(handle);

    WindowEvents::Resize eventData;
    eventData.width = width;
    eventData.height = height;

    window.events.Dispatch<void>(eventData).Unwrap();
    window.m_sizeChanged = true;
}

void Window::FocusCallback(GLFWwindow* handle, int focused)
{
    Window& window = GetWindowFromUserData(handle);

    WindowEvents::Focus eventData;
    eventData.focused = focused > 0;

    window.events.Dispatch<void>(eventData).Unwrap();
}

void Window::CloseCallback(GLFWwindow* handle)
{
    Window& window = GetWindowFromUserData(handle);

    window.events.Dispatch<void>(WindowEvents::Close{}).Unwrap();
}
