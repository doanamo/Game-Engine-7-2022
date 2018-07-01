/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "System/Context.hpp"
using namespace System;

namespace
{
    // Callback error function.
    void ErrorCallback(int error, const char* description)
    {
        LOG_ERROR() << "GLFW Error: " << description;
    }
}

Context::Context() :
    m_initialized(false)
{
}

Context::~Context()
{
    // Release GLFW library.
    if(m_initialized)
    {
        glfwTerminate();
    }
}

bool Context::Initialize()
{
    LOG() << "Initializing system context..." << LOG_INDENT();

    // Check if system context is already initialized.
    ASSERT(!m_initialized, "Context is already initialized!");

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
    LOG_INFO() << "Success!";

    return m_initialized = true;
}

bool Context::IsInitialized() const
{
    return m_initialized;
}
