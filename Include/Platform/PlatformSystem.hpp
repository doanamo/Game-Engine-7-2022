/*
    Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Core/System/EngineSystem.hpp>

/*
    Platform

    Main platform context that must be initialized first before other system classes can be used.
*/

namespace Platform
{
    class PlatformSystem final : public Core::EngineSystem
    {
        REFLECTION_ENABLE(Platform, Core::EngineSystem)

    public:
        PlatformSystem();
        ~PlatformSystem() override;

    private:
        bool OnAttach(const Core::EngineSystemStorage& engineSystems) override;

    private:
        bool m_attached = false;
    };
}

REFLECTION_TYPE(Platform::PlatformSystem, Core::EngineSystem)
