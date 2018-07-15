/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "System/Window.hpp"
using namespace System;

WindowInfo::WindowInfo() :
    title("Window"),
    width(1024),
    height(576),
    vsync(true),
    visible(true),
    minWidth(GLFW_DONT_CARE),
    minHeight(GLFW_DONT_CARE),
    maxWidth(GLFW_DONT_CARE),
    maxHeight(GLFW_DONT_CARE)
{
}

Window::Window() :
    m_handle(nullptr),
    m_title(""),
    m_sizeChanged(false)
{
}

Window::~Window()
{
    // We have to destroy an allocated handle.
    this->DestroyWindow();
}

void Window::DestroyWindow()
{
    // Destroy the window.
    if(m_handle != nullptr)
    {
        glfwDestroyWindow(m_handle);
        m_handle = nullptr;
    }
}

bool Window::Initialize(const WindowInfo& info)
{
    LOG() << "Initializing window..." << LOG_INDENT();

    // Check if instance is already initialized.
    VERIFY(m_handle == nullptr, "Window instance is already initialized!");

    // Setup a cleanup guard variable.
    bool initialized = false;

    // Validate arguments.
    if(info.width < 0)
    {
        LOG_ERROR() << "Invalid argument - \"info.width\" cannot be negative!";
        return false;
    }

    if(info.height < 0)
    {
        LOG_ERROR() << "Invalid argument - \"info.height\" cannot be negative!";
        return false;
    }

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

    // Show or hide the window after creation.
    glfwWindowHint(GLFW_VISIBLE, info.visible ? 1 : 0);

    // Create a window.
    // Bug: This function call can randomly take twice as much memory after a system call to SetPixelFormat().
    m_handle = glfwCreateWindow(info.width, info.height, info.title.c_str(), nullptr, nullptr);

    if(m_handle == nullptr)
    {
        LOG_ERROR() << "Could not create a window!";
        return false;
    }

    SCOPE_GUARD_IF(!initialized, this->DestroyWindow());

    // Set window size limits.
    glfwSetWindowSizeLimits(m_handle, info.minWidth, info.minHeight, info.maxWidth, info.maxHeight);

    // Set window user data.
    glfwSetWindowUserPointer(m_handle, this);

    // Add event callbacks.
    glfwSetWindowPosCallback(m_handle, Window::MoveCallback);
    glfwSetFramebufferSizeCallback(m_handle, Window::ResizeCallback);
    glfwSetWindowFocusCallback(m_handle, Window::FocusCallback);
    glfwSetWindowCloseCallback(m_handle, Window::CloseCallback);
    glfwSetKeyCallback(m_handle, Window::KeyboardKeyCallback);
    glfwSetCharCallback(m_handle, Window::TextInputCallback);
    glfwSetMouseButtonCallback(m_handle, Window::MouseButtonCallback);
    glfwSetScrollCallback(m_handle, Window::MouseScrollCallback);
    glfwSetCursorPosCallback(m_handle, Window::CursorPositionCallback);
    glfwSetCursorEnterCallback(m_handle, Window::CursorEnterCallback);

    // Make window context current.
    glfwMakeContextCurrent(m_handle);

    // Set the swap interval.
    glfwSwapInterval((int)info.vsync);

    // Initialize GLEW library for the current context.
    GLenum error = glewInit();

    if(error != GLEW_OK)
    {
        LOG_ERROR() << "GLEW Error: " << glewGetErrorString(error);
        LOG_ERROR() << "Could not initialize GLEW library!";
        return false;
    }

    ASSERT(glGetError() == GL_NO_ERROR, "OpenGL error occurred during context initialization!");

    // Log created window info.
    int windowWidth, windowHeight;
    glfwGetFramebufferSize(m_handle, &windowWidth, &windowHeight);

    LOG_INFO() << "Resolution is " << windowWidth << "x" << windowHeight << ".";

    // Log created OpenGL context.
    int glMajor = glfwGetWindowAttrib(m_handle, GLFW_CONTEXT_VERSION_MAJOR);
    int glMinor = glfwGetWindowAttrib(m_handle, GLFW_CONTEXT_VERSION_MINOR);

    LOG_INFO() << "Using OpenGL " << glMajor << "." << glMinor << " context.";

    // Store window's title as it cannot be retrieved back via GLFW.
    m_title = info.title;

    // Success!
    return initialized = true;
}

void Window::MakeContextCurrent()
{
    VERIFY(m_handle != nullptr, "Window instance is not initialized!");

    // Mark associated OpenGL context as current.
    glfwMakeContextCurrent(m_handle);
}

void Window::ProcessEvents()
{
    VERIFY(m_handle != nullptr, "Window instance is not initialized!");

    // Poll and process events using callbacks.
    glfwPollEvents();

    // Log window size change.
    if(m_sizeChanged)
    {
        int windowWidth, windowHeight;
        glfwGetFramebufferSize(m_handle, &windowWidth, &windowHeight);

        LOG_INFO() << "Window has been resized to " << windowWidth << "x" << windowHeight << ".";

        m_sizeChanged = false;
    }
}

void Window::Present()
{
    VERIFY(m_handle != nullptr, "Window instance is not initialized!");

    // Swap frame buffers.
    glfwSwapBuffers(m_handle);

    // Check if there are any uncaught OpenGL errors.
    GLenum error;
    while((error = glGetError()) != GL_NO_ERROR)
    {
        std::stringstream stream;
        stream << std::hex << std::setfill('0') << std::setw(4) << error;

        LOG_WARNING() << "Found uncaught OpenGL error in the last frame (code 0x" << stream.str() << ")!";
    }

    ASSERT(error == GL_NO_ERROR, "Uncaught OpenGL error(s) encountered!");
}

void Window::Close()
{
    VERIFY(m_handle != nullptr, "Window instance is not initialized!");

    // Force the window to close.
    glfwSetWindowShouldClose(m_handle, GL_TRUE);
}

void Window::MoveCallback(GLFWwindow* window, int x, int y)
{
    ASSERT(window != nullptr, "Window handle is invalid!");

    // Get the window instance.
    auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
    ASSERT(instance != nullptr, "Window instance is null!");

    // Send an event.
    Window::Events::Move eventData;
    eventData.x = x;
    eventData.y = y;

    instance->events.move(eventData);
}

void Window::ResizeCallback(GLFWwindow* window, int width, int height)
{
    ASSERT(window != nullptr, "Window reference is null!");

    // Get the window instance.
    auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
    ASSERT(instance != nullptr, "Window instance is null!");

    // Remember that window size has changed.
    instance->m_sizeChanged = true;

    // Send an event.
    Window::Events::Resize eventData;
    eventData.width = width;
    eventData.height = height;

    instance->events.resize(eventData);
}

void Window::FocusCallback(GLFWwindow* window, int focused)
{
    ASSERT(window != nullptr, "Window reference is null!");

    // Get the window instance.
    auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
    ASSERT(instance != nullptr, "Window instance is null!");

    // Send an event.
    Window::Events::Focus eventData;
    eventData.focused = focused > 0;

    instance->events.focus(eventData);
}

void Window::CloseCallback(GLFWwindow* window)
{
    ASSERT(window != nullptr, "Window reference is null!");

    // Get the window instance.
    auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
    ASSERT(instance != nullptr, "Window instance is null!");

    // Send an event.
    Window::Events::Close eventData;

    instance->events.close(eventData);
}

void Window::KeyboardKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    ASSERT(window != nullptr, "Window reference is null!");

    // Get the window instance.
    auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
    ASSERT(instance != nullptr, "Window instance is null!");

    // Send an event.
    Window::Events::KeyboardKey eventData;
    eventData.key = key;
    eventData.scancode = scancode;
    eventData.action = action;
    eventData.mods = mods;

    instance->events.keyboardKey(eventData);
}

void Window::TextInputCallback(GLFWwindow* window, unsigned int character)
{
    ASSERT(window != nullptr, "Window reference is null!");

    // Get the window instance.
    auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
    ASSERT(instance != nullptr, "Window instance is null!");

    // Send an event.
    Window::Events::TextInput eventData;
    eventData.character = character;

    instance->events.textInput(eventData);
}

void Window::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    ASSERT(window != nullptr, "Window reference is null!");

    // Get the window instance.
    auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
    ASSERT(instance != nullptr, "Window instance is null!");

    // Send an event.
    Window::Events::MouseButton eventData;
    eventData.button = button;
    eventData.action = action;
    eventData.mods = mods;

    instance->events.mouseButton(eventData);
}

void Window::MouseScrollCallback(GLFWwindow* window, double offsetx, double offsety)
{
    ASSERT(window != nullptr, "Window reference is null!");

    // Get the window instance.
    auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
    ASSERT(instance != nullptr, "Window instance is null!");

    // Send an event.
    Window::Events::MouseScroll eventData;
    eventData.offset = offsety;

    instance->events.mouseScroll(eventData);
}

void Window::CursorPositionCallback(GLFWwindow* window, double x, double y)
{
    ASSERT(window != nullptr, "Window reference is null!");

    // Get the window instance.
    auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
    ASSERT(instance != nullptr, "Window instance is null!");

    // Send an event.
    Window::Events::CursorPosition eventData;
    eventData.x = x;
    eventData.y = y;

    instance->events.cursorPosition(eventData);
}

void Window::CursorEnterCallback(GLFWwindow* window, int entered)
{
    ASSERT(window != nullptr, "Window reference is null!");

    // Get the window's instance.
    auto instance = reinterpret_cast<System::Window*>(glfwGetWindowUserPointer(window));
    ASSERT(instance != nullptr, "Window instance is null!");

    // Send an event.
    Window::Events::CursorEnter eventData;
    eventData.entered = entered != 0;

    instance->events.cursorEnter(eventData);
}

void Window::SetTitle(std::string title)
{
    VERIFY(m_handle != nullptr, "Window instance is not initialized!");

    // Set the window's new title.
    glfwSetWindowTitle(m_handle, title.c_str());

    // Cache the window's title as it cannot be retrieved back via GLFW.
    m_title = title;
}

void Window::SetVisibility(bool show)
{
    VERIFY(m_handle != nullptr, "Window instance is not initialized!");

    // Toggle visibility state.
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
    VERIFY(m_handle != nullptr, "Window instance is not initialized!");

    // Return the cached window's title.
    return m_title;
}

int Window::GetWidth() const
{
    VERIFY(m_handle != nullptr, "Window instance is not initialized!");

    // Return the current framebuffer's width.
    int width = 0;
    glfwGetFramebufferSize(m_handle, &width, nullptr);
    return width;
}

int Window::GetHeight() const
{
    VERIFY(m_handle != nullptr, "Window instance is not initialized!");

    // Return the current framebuffer's height.
    int height = 0;
    glfwGetFramebufferSize(m_handle, nullptr, &height);
    return height;
}

bool Window::IsOpen() const
{
    VERIFY(m_handle != nullptr, "Window instance is not initialized!");

    // Window is considered open as long as there was no request made to close it.
    return glfwWindowShouldClose(m_handle) == 0;
}

bool Window::IsFocused() const
{
    VERIFY(m_handle != nullptr, "Window instance is not initialized!");

    // Check if the window is currently in the foreground.
    return glfwGetWindowAttrib(m_handle, GLFW_FOCUSED) > 0;
}

bool Window::IsValid() const
{
    return m_handle != nullptr;
}

GLFWwindow* Window::GetPrivateHandle()
{
    VERIFY(m_handle != nullptr, "Window instance is not initialized!");

    // Return the private window handle.
    return m_handle;
}
