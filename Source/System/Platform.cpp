/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "System/Platform.hpp"
using namespace System;

namespace
{
    void ErrorCallback(int error, const char* description)
    {
        LOG_ERROR("GLFW Error: {}", description);
    }
}

int Platform::InstanceCounter = 0;

Platform::Platform() = default;

Platform::~Platform()
{
    ASSERT(InstanceCounter > 0, "Invalid instance count!");

    // Decrement instance counter.
    InstanceCounter--;

    // Terminate GLFW library when the counter reaches zero.
    if(InstanceCounter == 0)
    {
        glfwTerminate();
    }
}

Platform::CreateResult Platform::Create()
{
    LOG("Creating platform...");
    LOG_SCOPED_INDENT();

    // Initialize GLFW library if this is the first or only instance.
    if(InstanceCounter == 0)
    {
        // Set callback function for future GLFW errors.
        glfwSetErrorCallback(ErrorCallback);

        // Initialize GLFW library.
        if(!glfwInit())
        {
            LOG_ERROR("Could not initialize GLFW library!");
            return Common::Failure(CreateErrors::FailedGlfwInitialization);
        }

        // Write GLFW details to log.
        int major, minor, revision;
        glfwGetVersion(&major, &minor, &revision);
        LOG_INFO("Using GLFW {}.{}.{} library.", major, minor, revision);

        // Increment instance counter.
        InstanceCounter++;
    }

    // Create instance that will terminate GLFW library when needed.
    auto instance = std::unique_ptr<Platform>(new Platform());
    
    // Success!
    return Common::Success(std::move(instance));
}
