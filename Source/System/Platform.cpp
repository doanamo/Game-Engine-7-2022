/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "System/Platform.hpp"
using namespace System;

namespace
{
    // GLFW initialization guard counter.
    static int GLFWRefCounter = 0;

    void TerminateGLFW()
    {
        // Decrement the reference counter.
        GLFWRefCounter--;

        // Release GLFW library when the counter reaches zero.
        if(GLFWRefCounter == 0)
        {
            glfwTerminate();
        }
    }
}

namespace
{
    void ErrorCallback(int error, const char* description)
    {
        LOG_ERROR("GLFW Error: {}", description);
    }
}

Platform::~Platform()
{
    // Release GLFW library.
    if(m_initialized)
    {
        TerminateGLFW();
    }
}

Platform::Platform(Platform&& other) :
    Platform()
{
    *this = std::move(other);
}

Platform& Platform::operator=(Platform&& other)
{
    std::swap(m_initialized, other.m_initialized);
    return *this;
}

bool Platform::Initialize()
{
    LOG("Initializing platform...");
    LOG_SCOPED_INDENT();

    // Check if system context is already initialized.
    ASSERT(!m_initialized, "Platform is already initialized!");

    // Initialize GLFW library only once when the reference counter is zero.
    if(GLFWRefCounter == 0)
    {
        // Set a callback function for future GLFW errors.
        glfwSetErrorCallback(ErrorCallback);

        // Initialize GLFW library.
        if(!glfwInit())
        {
            LOG_ERROR("Could not initialize GLFW library!");
            return false;
        }

        GLFWRefCounter++;
    }

    // Write GLFW details to log.
    int major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);

    LOG_INFO("Using GLFW {}.{}.{} library.", major, minor, revision);

    // Success!
    return m_initialized = true;
}

bool Platform::IsInitialized() const
{
    return m_initialized;
}
