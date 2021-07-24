/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include "Core/SystemInterface.hpp"

/*
    Engine System

    Base class for engine systems to be used with system storage.
*/

namespace Core
{
    class EngineSystem : public SystemInterface<EngineSystem>
    {
        REFLECTION_ENABLE(EngineSystem)

    public:
        virtual ~EngineSystem() = default;

    protected:
        EngineSystem() = default;

        bool OnAttach(const SystemStorage<EngineSystem>& engineSystems) override
        {
            // Return true to signal that everything is fine and we can continue.
            return true;
        }
    };

    using EngineSystemStorage = SystemStorage<EngineSystem>;
}

REFLECTION_TYPE(Core::EngineSystem)
