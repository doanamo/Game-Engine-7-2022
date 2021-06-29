/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Core/EngineSystem.hpp>
#include "Editor/EditorSubsystem.hpp"
#include "Editor/Modules/InputManagerEditor.hpp"
#include "Editor/Modules/GameInstanceEditor.hpp"

namespace Core
{
    class PerformanceMetrics;
};

namespace System
{
    class Window;
};

namespace Game
{
    class GameFramework;
}

/*
    Editor Shell

    Main front end class for editor interface.
*/

namespace Editor
{
    class EditorShell final : private EditorSubsystem
    {
        REFLECTION_ENABLE(EditorShell, EditorSubsystem)

    public:
        EditorShell();
        ~EditorShell();

    private:
        bool OnAttach(const EditorSubsystemStorage& editorSubsystems) override;
        bool CreateModules(const Core::EngineSystemStorage& engineSystems);

        void OnBeginInterface(float timeDelta) override;
        void DisplayMenuBar();
        void DisplayFramerate();

    private:
        Core::PerformanceMetrics* m_performanceMetrics = nullptr;
        System::Window* m_window = nullptr;

        std::unique_ptr<InputManagerEditor> m_inputManagerEditor;
        std::unique_ptr<GameInstanceEditor> m_gameInstanceEditor;

        bool m_showDemoWindow = false;
    };
}

REFLECTION_TYPE(Editor::EditorShell, Editor::EditorSubsystem)
