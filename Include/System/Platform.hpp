/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Core/Service.hpp>

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
    class Platform final : public Core::Service
    {
        REFLECTION_ENABLE(Platform, Core::Service)

    public:
        enum class CreateErrors
        {
            FailedGlfwInitialization,
        };

        using CreateResult = Common::Result<std::unique_ptr<Platform>, CreateErrors>;
        static CreateResult Create();

    public:
        ~Platform() override;

    private:
        Platform();

    private:
        static int InstanceCounter;
    };
}

REFLECTION_TYPE(System::Platform, Core::Service)
