/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    Platform

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
    class Platform final : private NonCopyable, public Resettable<Platform>
    {
    public:
        enum class InitializeErrors
        {
            FailedGlfwInitialization,
        };

        using InitializeResult = Result<void, InitializeErrors>;

    public:
        Platform();
        ~Platform();

        InitializeResult Initialize();
        bool IsInitialized() const;

    private:
        bool m_initialized = false;
    };
}
