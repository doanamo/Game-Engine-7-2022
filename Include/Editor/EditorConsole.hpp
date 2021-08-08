/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Core/EngineSystem.hpp>
#include <System/InputDefinitions.hpp>
#include "Editor/EditorSubsystem.hpp"

namespace System
{
    class Window;
}

/*
    Editor Console

    Interface for console window with log output.
*/

namespace Editor
{
    class EditorConsole final : private EditorSubsystem
    {
        REFLECTION_ENABLE(EditorConsole, EditorSubsystem)

    public:
        EditorConsole();
        ~EditorConsole();

        void Toggle(bool visibility);
        bool IsVisible() const;

    private:
        bool OnAttach(const EditorSubsystemStorage& editorSubsystems) override;
        bool OnKeyboardKey(const System::InputEvents::KeyboardKey& event) override;
        void OnBeginInterface(float timeDelta) override;

    private:
        System::Window* m_window = nullptr;

        bool m_visible = false;
        bool m_autoScroll = true;
        std::string m_inputBuffer;
    };
}

REFLECTION_TYPE(Editor::EditorConsole, Editor::EditorSubsystem)
