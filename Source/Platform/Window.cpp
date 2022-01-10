
/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Platform/Window.hpp"
#include <Core/Build/Build.hpp>
using namespace Platform;

namespace
{
    const char* LogCreateFailed = "Failed to create window! {}";
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

Window::CreateResult Window::Create(const CreateParams& params)
{
    LOG_PROFILE_SCOPE_FUNC();

    // Check parameters.
    CHECK_ARGUMENT_OR_RETURN(params.width >= 0 && params.height >= 0,
        Common::Failure(CreateErrors::InvalidSize));

    // Create window instance.
    auto instance = std::unique_ptr<Window>(new Window());

    // Setup window hints.
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);

#ifdef PLATFORM_EMSCRIPTEN
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

    // Append additional information to window title in non-release configurations.
    std::string windowTitle = params.title;

#if !defined(CONFIG_RELEASE)
    windowTitle = fmt::format("{} ({}, PID: {})", windowTitle,
        Build::GetConfig(), Debug::GetProcessID());
#endif

    // Create window.
    instance->m_context.handle = glfwCreateWindow(
        params.width, params.height, windowTitle.c_str(), nullptr, nullptr);
    if( instance->m_context.handle == nullptr)
    {
        LOG_ERROR(LogCreateFailed, "Could not create window.");
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

    // Save window parameters.
    glfwGetFramebufferSize(instance->m_context.handle, &instance->m_width, &instance->m_height);
    LOG_INFO("Resolution is {}x{}.", instance->m_width, instance->m_height);

    instance->m_title = params.title;

    // Prepare OpenGL function and extension loader.
    if(!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress))
    {
        LOG_ERROR(LogCreateFailed, "Could not load OpenGL function and extension loader.");
        return Common::Failure(CreateErrors::OpenGLLoaderError);
    }

    ASSERT(glGetError() == GL_NO_ERROR, "OpenGL error occurred during context initialization!");

    // Log version of created OpenGL context.
    int glInterface = glfwGetWindowAttrib(instance->m_context.handle, GLFW_CLIENT_API);
    int glMajor = glfwGetWindowAttrib(instance->m_context.handle, GLFW_CONTEXT_VERSION_MAJOR);
    int glMinor = glfwGetWindowAttrib(instance->m_context.handle, GLFW_CONTEXT_VERSION_MINOR);
    LOG_INFO("Using OpenGL {}{}.{} context.",
        glInterface == GLFW_OPENGL_API ? "" : "ES ", glMajor, glMinor);

    return Common::Success(std::move(instance));
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
    return glfwWindowShouldClose(m_context.handle) != 0;
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
