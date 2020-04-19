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
*/

namespace System
{
    class Platform final : private NonCopyable
    {
    public:
        enum class CreateErrors
        {
            FailedGlfwInitialization,
        };

        using CreateResult = Result<std::unique_ptr<Platform>, CreateErrors>;
        static CreateResult Create();

    public:
        ~Platform();

    private:
        Platform();

    private:
        static int InstanceCounter;
    };
}
