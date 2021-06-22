/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Core/Service.hpp>

/*
    Platform

    Main platform context that must be initialized first before other system classes can be used.
*/

namespace System
{
    class Platform final : public Core::Service
    {
        REFLECTION_ENABLE(Platform, Core::Service)

    public:
        Platform();
        ~Platform() override;

    private:
        bool OnAttach(const Core::ServiceStorage* services) override;

        bool m_attached = false;
    };
}

REFLECTION_TYPE(System::Platform, Core::Service)
