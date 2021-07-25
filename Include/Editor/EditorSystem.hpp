/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Core/EngineSystem.hpp>
#include <Core/SystemStorage.hpp>
#include <Common/Event/EventReceiver.hpp>
#include <System/InputDefinitions.hpp>
#include "Editor/EditorSubsystem.hpp"

namespace System
{
    class Timer;
    class Window;
};

namespace Editor
{
    class EditorRenderer;
    class EditorConsole;
    class EditorShell;
}

/*
    Editor System

    Displays and handles different editor interfaces.
*/

namespace Editor
{
    class EditorSystem final : public Core::EngineSystem
    {
        REFLECTION_ENABLE(EditorSystem, Core::EngineSystem)

    public:
        EditorSystem();
        ~EditorSystem() override;

    private:
        bool OnAttach(const Core::EngineSystemStorage& engineSystems) override;
        void OnBeginFrame() override;
        void OnEndFrame() override;

        bool CreateContext();
        bool CreateSubsystems(const Core::EngineSystemStorage& engineSystems);
        bool SubscribeEvents(const Core::EngineSystemStorage& engineSystems);

        bool OnTextInput(const System::InputEvents::TextInput& event);
        bool OnKeyboardKey(const System::InputEvents::KeyboardKey& event);
        bool OnMouseButton(const System::InputEvents::MouseButton& event);
        bool OnMouseScroll(const System::InputEvents::MouseScroll& event);
        void OnCursorPosition(const System::InputEvents::CursorPosition& event);

        Event::Receiver<void(const System::InputEvents::CursorPosition&)> m_receiverCursorPosition;
        Event::Receiver<bool(const System::InputEvents::MouseButton&)> m_receiverMouseButton;
        Event::Receiver<bool(const System::InputEvents::MouseScroll&)> m_receiverMouseScroll;
        Event::Receiver<bool(const System::InputEvents::KeyboardKey&)> m_receiverKeyboardKey;
        Event::Receiver<bool(const System::InputEvents::TextInput&)> m_receiverTextInput;

    private:
        System::Timer* m_timer = nullptr;
        System::Window* m_window = nullptr;
        ImGuiContext* m_interface = nullptr;

        EditorSubsystemStorage m_subsystems;
    };
}

REFLECTION_TYPE(Editor::EditorSystem, Core::EngineSystem)
