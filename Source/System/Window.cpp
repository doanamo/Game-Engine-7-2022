/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "System/Window.hpp"
using namespace System;

Window::Window() = default;

Window::~Window()
{
    if(m_handle)
    {
        glfwDestroyWindow(m_handle);
        m_handle = nullptr;
    }
}

Window::CreateResult Window::Create(const CreateFromParams& params)
{
    LOG("Creating window...");
    LOG_SCOPED_INDENT();

    // Check arguments.
    CHECK_ARGUMENT_OR_RETURN(params.width >= 0, Common::Failure(CreateErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(params.height >= 0, Common::Failure(CreateErrors::InvalidArgument));

    // Create instance.
    auto instance = std::unique_ptr<Window>(new Window());

    // Setup window hints.
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Show or hide window after creation.
    glfwWindowHint(GLFW_VISIBLE, params.visible ? 1 : 0);

    // Create window.
    instance->m_handle = glfwCreateWindow(params.width, params.height, params.title.c_str(), nullptr, nullptr);

    if(instance->m_handle == nullptr)
    {
        LOG_ERROR("Could not create GLFW window!");
        return Common::Failure(CreateErrors::FailedWindowCreation);
    }

    // Set window size limits.
    glfwSetWindowSizeLimits(instance->m_handle, params.minWidth, params.minHeight, params.maxWidth, params.maxHeight);

    // Set window user data.
    glfwSetWindowUserPointer(instance->m_handle, instance.get());

    // Add event callbacks.
    glfwSetWindowPosCallback(instance->m_handle, Window::MoveCallback);
    glfwSetFramebufferSizeCallback(instance->m_handle, Window::ResizeCallback);
    glfwSetWindowFocusCallback(instance->m_handle, Window::FocusCallback);
    glfwSetWindowCloseCallback(instance->m_handle, Window::CloseCallback);
    glfwSetKeyCallback(instance->m_handle, Window::KeyboardKeyCallback);
    glfwSetCharCallback(instance->m_handle, Window::TextInputCallback);
    glfwSetMouseButtonCallback(instance->m_handle, Window::MouseButtonCallback);
    glfwSetScrollCallback(instance->m_handle, Window::MouseScrollCallback);
    glfwSetCursorPosCallback(instance->m_handle, Window::CursorPositionCallback);
    glfwSetCursorEnterCallback(instance->m_handle, Window::CursorEnterCallback);

    // Make window context current.
    glfwMakeContextCurrent(instance->m_handle);

    // Set swap interval.
    glfwSwapInterval((int)params.vsync);

    // Initialize GLEW library for the current context.
    GLenum error = glewInit();

    if(error != GLEW_OK)
    {
        LOG_ERROR("GLEW Error: {}", glewGetErrorString(error));
        LOG_ERROR("Could not initialize GLEW library!");
        return Common::Failure(CreateErrors::FailedGlewInitialization);
    }

    ASSERT(glGetError() == GL_NO_ERROR, "OpenGL error occurred during context initialization!");

    // Log created window info.
    int windowWidth, windowHeight;
    glfwGetFramebufferSize(instance->m_handle, &windowWidth, &windowHeight);
    LOG_INFO("Resolution is {}x{}.", windowWidth, windowHeight);

    // Log created OpenGL context.
    int glMajor = glfwGetWindowAttrib(instance->m_handle, GLFW_CONTEXT_VERSION_MAJOR);
    int glMinor = glfwGetWindowAttrib(instance->m_handle, GLFW_CONTEXT_VERSION_MINOR);
    LOG_INFO("Using OpenGL {}.{} context.", glMajor, glMinor);

    // Store window title as it cannot be retrieved back via GLFW.
    instance->m_title = params.title;

    // Success!
    return Common::Success(std::move(instance));
}

void Window::MakeContextCurrent()
{
    // Mark associated OpenGL context as current.
    glfwMakeContextCurrent(m_handle);
}

void Window::ProcessEvents()
{
    // Poll and process events using callbacks.
    glfwPollEvents();

    // Log window size change.
    if(m_sizeChanged)
    {
        int windowWidth, windowHeight;
        glfwGetFramebufferSize(m_handle, &windowWidth, &windowHeight);
        LOG_INFO("Window has been resized to {}x{}.", windowWidth, windowHeight);

        m_sizeChanged = false;
    }
}

void Window::Present()
{
    // Swap frame buffers and check if there are any uncaught OpenGL errors.
    glfwSwapBuffers(m_handle);

    GLenum error;
    while((error = glGetError()) != GL_NO_ERROR)
    {
        LOG_WARNING("Found uncaught OpenGL error in the last frame (code {:#06x})!", error);
    }

    ASSERT(error == GL_NO_ERROR, "Uncaught OpenGL error(s) encountered!");
}

void Window::Close()
{
    glfwSetWindowShouldClose(m_handle, GL_TRUE);
}

void Window::SetTitle(std::string title)
{
    glfwSetWindowTitle(m_handle, title.c_str());
    m_title = title;
}

void Window::SetVisibility(bool show)
{
    if(show)
    {
        glfwShowWindow(m_handle);
    }
    else
    {
        glfwHideWindow(m_handle);
    }
}

std::string Window::GetTitle() const
{
    return m_title;
}

int Window::GetWidth() const
{
    int width = 0;
    glfwGetFramebufferSize(m_handle, &width, nullptr);
    return width;
}

int Window::GetHeight() const
{
    int height = 0;
    glfwGetFramebufferSize(m_handle, nullptr, &height);
    return height;
}

bool Window::IsOpen() const
{
    // Window is considered open as long as there was no request made to close it.
    return glfwWindowShouldClose(m_handle) == 0;
}

bool Window::IsFocused() const
{
    return glfwGetWindowAttrib(m_handle, GLFW_FOCUSED) > 0;
}

GLFWwindow* Window::GetPrivateHandle()
{
    return m_handle;
}

void Window::MoveCallback(GLFWwindow* window, int x, int y)
{
    ASSERT(window != nullptr, "Window handle is invalid!");

    auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
    ASSERT(instance != nullptr, "Window instance is null!");

    Window::Events::Move eventData;
    eventData.x = x;
    eventData.y = y;
    instance->events.move(eventData);
}

void Window::ResizeCallback(GLFWwindow* window, int width, int height)
{
    ASSERT(window != nullptr, "Window reference is null!");

    auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
    ASSERT(instance != nullptr, "Window instance is null!");

    Window::Events::Resize eventData;
    eventData.width = width;
    eventData.height = height;
    instance->events.resize(eventData);

    // Remember that window size has changed.
    instance->m_sizeChanged = true;
}

void Window::FocusCallback(GLFWwindow* window, int focused)
{
    ASSERT(window != nullptr, "Window reference is null!");

    auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
    ASSERT(instance != nullptr, "Window instance is null!");

    Window::Events::Focus eventData;
    eventData.focused = focused > 0;
    instance->events.focus(eventData);
}

void Window::CloseCallback(GLFWwindow* window)
{
    ASSERT(window != nullptr, "Window reference is null!");

    auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
    ASSERT(instance != nullptr, "Window instance is null!");

    Window::Events::Close eventData;
    instance->events.close(eventData);
}

void Window::KeyboardKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    ASSERT(window != nullptr, "Window reference is null!");

    auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
    ASSERT(instance != nullptr, "Window instance is null!");

    Window::Events::KeyboardKey eventData;
    eventData.key = key;
    eventData.scancode = scancode;
    eventData.action = action;
    eventData.modifiers = mods;
    instance->events.keyboardKey(eventData);
}

void Window::TextInputCallback(GLFWwindow* window, unsigned int character)
{
    ASSERT(window != nullptr, "Window reference is null!");

    auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
    ASSERT(instance != nullptr, "Window instance is null!");

    Window::Events::TextInput eventData;
    eventData.utf32Character = character;
    instance->events.textInput(eventData);
}

void Window::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    ASSERT(window != nullptr, "Window reference is null!");

    auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
    ASSERT(instance != nullptr, "Window instance is null!");

    Window::Events::MouseButton eventData;
    eventData.button = button;
    eventData.action = action;
    eventData.modifiers = mods;
    instance->events.mouseButton(eventData);
}

void Window::MouseScrollCallback(GLFWwindow* window, double offsetx, double offsety)
{
    ASSERT(window != nullptr, "Window reference is null!");

    auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
    ASSERT(instance != nullptr, "Window instance is null!");

    Window::Events::MouseScroll eventData;
    eventData.offset = offsety;
    instance->events.mouseScroll(eventData);
}

void Window::CursorPositionCallback(GLFWwindow* window, double x, double y)
{
    ASSERT(window != nullptr, "Window reference is null!");

    auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
    ASSERT(instance != nullptr, "Window instance is null!");

    Window::Events::CursorPosition eventData;
    eventData.x = x;
    eventData.y = y;
    instance->events.cursorPosition(eventData);
}

void Window::CursorEnterCallback(GLFWwindow* window, int entered)
{
    ASSERT(window != nullptr, "Window reference is null!");

    auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
    ASSERT(instance != nullptr, "Window instance is null!");

    Window::Events::CursorEnter eventData;
    eventData.entered = entered != 0;
    instance->events.cursorEnter(eventData);
}
