/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Core/System/EngineSystem.hpp>
#include <Platform/InputDefinitions.hpp>
#include "Editor/EditorSubsystem.hpp"

namespace Platform
{
    class WindowSystem;
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
        bool OnKeyboardKey(const Platform::InputEvents::KeyboardKey& event) override;
        void OnBeginInterface(float timeDelta) override;

    private:
        Platform::WindowSystem* m_windowSystem = nullptr;

        std::string m_copyBuffer;
        std::string m_inputBuffer;

        bool m_windowVisible = false;
        bool m_resetScroll = false;
        bool m_autoScroll = true;
        bool m_pause = false;

        bool m_severityFilters[Logger::Severity::Count];
    };
}

REFLECTION_TYPE(Editor::EditorConsole, Editor::EditorSubsystem)
