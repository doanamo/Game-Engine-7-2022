/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Core/SystemInterface.hpp>
#include <Core/EngineSystem.hpp>

namespace System::InputEvents
{
    struct KeyboardKey;
}

/*
    Editor Subsystem

    Base class for editor subsystems to be used with system storage.
*/

namespace Editor
{
    class EditorSubsystem : public Core::SystemInterface<EditorSubsystem>
    {
        REFLECTION_ENABLE(EditorSubsystem)

    public:
        virtual ~EditorSubsystem() = default;

        virtual void OnBeginInterface(float timeDelta) {}
        virtual void OnEndInterface() {}

        virtual bool OnKeyboardKey(const System::InputEvents::KeyboardKey& event)
        {
            // Return false if we do not intend on capturing input event.
            return false;
        }

    protected:
        EditorSubsystem() = default;
    };

    using EditorSubsystemStorage = Core::SystemStorage<EditorSubsystem>;
}

REFLECTION_TYPE(Editor::EditorSubsystem)

/*
    Editor Subsystem Context

    Utility for retrieving engine system storage from editor subsystems.
*/

namespace Editor
{
    class EditorSubsystemContext : public EditorSubsystem
    {
        REFLECTION_ENABLE(EditorSubsystemContext, EditorSubsystem)

    public:
        EditorSubsystemContext(const Core::EngineSystemStorage& engineSystems)
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

REFLECTION_TYPE(Editor::EditorSubsystemContext, Editor::EditorSubsystem)
