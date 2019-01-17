/*
    Copyright (c) 2018-2019 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Common/NonCopyable.hpp"

/*
    System Platform

    Main platform context that must be initialized
    first before other system classes can be used.

    This class encapsulated initialization routines
    that must be called at the very beginning of
    the main() function of the main thread.

    void ExmapleSystemPlatform()
    {
        // Initialize the platform context.
        System::Platform platform;
        if(!platform.Initialize())
            return;

        // Now all system classes can be used.
        System::Window window;
        System::Timer timer;
    }
*/

namespace System
{
    // Platform class.
    class Platform
    {
    public:
        Platform();
        ~Platform();

        // Disallow copy operations.
        Platform(const Platform& other) = delete;
        Platform& operator=(const Platform& other) = delete;

        // Implement move operations.
        Platform(Platform&& other);
        Platform& operator=(Platform&& other);

        // Initializes the platform context.
        bool Initialize();

        // Checks if the instance is initialized.
        bool IsInitialized() const;

    private:
        // Initialization state.
        bool m_initialized;
    };
}
