/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "System/Precompiled.hpp"
#include "System/Window.hpp"
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

Window::CreateResult Window::Create(const CreateFromParams& params)
{
    LOG("Creating window...");
    LOG_SCOPED_INDENT();

    CHECK_ARGUMENT_OR_RETURN(params.width >= 0, Common::Failure(CreateErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(params.height >= 0, Common::Failure(CreateErrors::InvalidArgument));

    auto instance = std::unique_ptr<Window>(new Window());
    instance->m_title = params.title;

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

    glfwWindowHint(GLFW_VISIBLE, params.visible ? 1 : 0);

    instance->m_context.handle = glfwCreateWindow(
        params.width, params.height, params.title.c_str(), nullptr, nullptr);
    if(instance->m_context.handle == nullptr)
    {
        LOG_ERROR("Could not create GLFW window!");
        return Common::Failure(CreateErrors::FailedWindowCreation);
    }

    glfwSetWindowUserPointer(instance->m_context.handle, &instance->m_context);
    glfwSetWindowSizeLimits(instance->m_context.handle,
        params.minWidth, params.minHeight, params.maxWidth, params.maxHeight);

    glfwSetWindowPosCallback(instance->m_context.handle, Window::MoveCallback);
    glfwSetFramebufferSizeCallback(instance->m_context.handle, Window::ResizeCallback);
    glfwSetWindowFocusCallback(instance->m_context.handle, Window::FocusCallback);
    glfwSetWindowCloseCallback(instance->m_context.handle, Window::CloseCallback);

    glfwMakeContextCurrent(instance->m_context.handle);
    glfwSwapInterval((int)params.vsync);

    if(!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress))
    {
        LOG_ERROR("Could not load OpenGL ES extensions!");
        return Common::Failure(CreateErrors::FailedExtensionLoad);
    }

    ASSERT(glGetError() == GL_NO_ERROR, "OpenGL error occurred during context initialization!");

    int windowWidth, windowHeight;
    glfwGetFramebufferSize(instance->m_context.handle, &windowWidth, &windowHeight);
    LOG_INFO("Resolution is {}x{}.", windowWidth, windowHeight);

    int glInterface = glfwGetWindowAttrib(instance->m_context.handle, GLFW_CLIENT_API);
    int glMajor = glfwGetWindowAttrib(instance->m_context.handle, GLFW_CONTEXT_VERSION_MAJOR);
    int glMinor = glfwGetWindowAttrib(instance->m_context.handle, GLFW_CONTEXT_VERSION_MINOR);
    LOG_INFO("Using OpenGL {}{}.{} context.",
        glInterface == GLFW_OPENGL_API ? "" : "ES ", glMajor, glMinor);

    return Common::Success(std::move(instance));
}

void Window::MakeContextCurrent()
{
    glfwMakeContextCurrent(m_context.handle);
}

void Window::ProcessEvents()
{
    glfwPollEvents();

    if(m_sizeChanged)
    {
        int windowWidth, windowHeight;
        glfwGetFramebufferSize(m_context.handle, &windowWidth, &windowHeight);
        LOG_INFO("Window has been resized to {}x{}.", windowWidth, windowHeight);

        m_sizeChanged = false;
    }
}

void Window::Present()
{
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
    glfwSetWindowShouldClose(m_context.handle, GL_TRUE);
}

void Window::SetTitle(std::string title)
{
    glfwSetWindowTitle(m_context.handle, title.c_str());
    m_title = title;
}

void Window::SetVisibility(bool show)
{
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
    glfwGetFramebufferSize(m_context.handle, &width, nullptr);
    return width;
}

int Window::GetHeight() const
{
    int height = 0;
    glfwGetFramebufferSize(m_context.handle, nullptr, &height);
    return height;
}

bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(m_context.handle) == 0;
}

bool Window::IsFocused() const
{
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
