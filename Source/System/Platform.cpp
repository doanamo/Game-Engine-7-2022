/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "System/Platform.hpp"
using namespace System;

namespace
{
    // Callback error function.
    void ErrorCallback(int error, const char* description)
    {
        LOG_ERROR() << "GLFW Error: " << description;
    }
}

Platform::Platform() :
    m_initialized(false)
{
}

Platform::~Platform()
{
    // Release GLFW library.
    if(m_initialized)
    {
        glfwTerminate();
    }
}

bool Platform::Initialize()
{
    LOG() << "Initializing platform..." << LOG_INDENT();

    // Check if system context is already initialized.
    ASSERT(!m_initialized, "Platform is already initialized!");

    // Set a callback function for future GLFW errors.
    glfwSetErrorCallback(ErrorCallback);

    // Initialize GLFW library.
    if(!glfwInit())
    {
        LOG_ERROR() << "Could not initialize GLFW library!";
        return false;
    }

    // Write GLFW details to log.
    int major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);

    LOG_INFO() << "Using GLFW " << major << "." << minor << "." << revision << " library.";

    // Success!
    LOG_DEBUG() << "Success!";

    return m_initialized = true;
}

bool Platform::IsInitialized() const
{
    return m_initialized;
}
