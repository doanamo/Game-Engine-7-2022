/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "System/Platform.hpp"
using namespace System;

namespace
{
    // GLFW initialization guard counter.
    static int GlfwRefCounter = 0;

    void ErrorCallback(int error, const char* description)
    {
        LOG_ERROR("GLFW Error: {}", description);
    }
}

Platform::Platform() = default;

Platform::~Platform()
{
    // Release reference to GLFW library.
    if(m_initialized)
    {
        // Decrement reference counter.
        GlfwRefCounter--;

        // Terminate GLFW library when the counter reaches zero.
        if(GlfwRefCounter == 0)
        {
            glfwTerminate();
        }
    }
}

Platform::InitializeResult Platform::Initialize()
{
    LOG("Initializing platform...");
    LOG_SCOPED_INDENT();

    // Setup initialization guard.
    VERIFY(!m_initialized, "Instance has already been initialized!");
    SCOPE_GUARD_IF(!m_initialized, this->Reset());

    // Initialize GLFW library only once when the reference counter is zero.
    if(GlfwRefCounter == 0)
    {
        // Set callback function for future GLFW errors.
        glfwSetErrorCallback(ErrorCallback);

        // Initialize GLFW library.
        if(!glfwInit())
        {
            LOG_ERROR("Could not initialize GLFW library!");
            return Failure(InitializeErrors::FailedGlfwInitialization);
        }

        GlfwRefCounter++;
    }

    // Write GLFW details to log.
    int major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);
    LOG_INFO("Using GLFW {}.{}.{} library.", major, minor, revision);

    // Success!
    m_initialized = true;
    return Success();
}

bool Platform::IsInitialized() const
{
    return m_initialized;
}
