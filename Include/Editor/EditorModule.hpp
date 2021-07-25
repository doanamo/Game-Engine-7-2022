/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Core/SystemInterface.hpp>
#include <Core/EngineSystem.hpp>

/*
    Editor Module

    Base class for editor modules to be used with system storage.
*/

namespace Editor
{
    class EditorModule : public Core::SystemInterface<EditorModule>
    {
        REFLECTION_ENABLE(EditorModule)

    public:
        virtual ~EditorModule() = default;
        virtual void OnDisplay(float timeDelta) {}
        virtual void OnDisplayMenuBar() {}

    protected:
        EditorModule() = default;
    };

    using EditorModuleStorage = Core::SystemStorage<EditorModule>;
}

REFLECTION_TYPE(Editor::EditorModule)

/*
    Editor Module Context

    Utility for retrieving engine system storage from editor modules.
*/

namespace Editor
{
    class EditorModuleContext : public EditorModule
    {
        REFLECTION_ENABLE(EditorModuleContext, EditorModule)

    public:
        EditorModuleContext(const Core::EngineSystemStorage& engineSystems)
            : m_engineSystems(engineSystems)
        {
        }

        const Core::EngineSystemStorage& GetEngineSystems() const
        {
            return m_engineSystems;
        }

    private:
        const Core::EngineSystemStorage& m_engineSystems;
    };
}

REFLECTION_TYPE(Editor::EditorModuleContext, Editor::EditorModule)
