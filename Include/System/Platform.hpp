/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
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
        // Initialize the platform instance.
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
    class Platform : private NonCopyable
    {
    public:
        Platform();
        ~Platform();

        // Initializes the platform context.
        bool Initialize();

        // Checks if the instance is initialized.
        bool IsInitialized() const;

    private:
        // Initialization state.
        bool m_initialized;
    };
}
