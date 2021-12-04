/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include "Core/System/SystemInterface.hpp"
#include "Core/Config/ConfigTypes.hpp"

/*
    Engine System

    Base class for engine systems to be used with system storage.
*/

namespace Core
{
    struct EngineStorageContext
    {
        ConfigVariableArray initialConfigVars;
    };

    class EngineSystem : public SystemInterface<EngineSystem, EngineStorageContext>
    {
        REFLECTION_ENABLE(EngineSystem)

    public:
        virtual ~EngineSystem() = default;
        virtual void OnRunEngine() {}
        virtual void OnPreFrame() {}
        virtual void OnBeginFrame() {}
        virtual void OnProcessFrame() {}
        virtual void OnEndFrame() {}
        virtual void OnPostFrame() {}

        virtual bool IsRequestingExit()
        {
            return false;
        }

    protected:
        EngineSystem() = default;
    };

    using EngineSystemStorage = SystemStorage<EngineSystem, EngineStorageContext>;
}

REFLECTION_TYPE(Core::EngineSystem)
