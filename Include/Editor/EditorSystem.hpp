/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Core/System/EngineSystem.hpp>
#include <Core/System/SystemStorage.hpp>
#include <Common/Event/EventReceiver.hpp>
#include <Platform/InputDefinitions.hpp>
#include "Editor/EditorSubsystem.hpp"

namespace Platform
{
    class TimerSystem;
    class WindowSystem;
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

        bool OnTextInput(const Platform::InputEvents::TextInput& event);
        bool OnKeyboardKey(const Platform::InputEvents::KeyboardKey& event);
        bool OnMouseButton(const Platform::InputEvents::MouseButton& event);
        bool OnMouseScroll(const Platform::InputEvents::MouseScroll& event);
        void OnCursorPosition(const Platform::InputEvents::CursorPosition& event);

        Event::Receiver<void(const Platform::InputEvents::CursorPosition&)> m_receiverCursorPosition;
        Event::Receiver<bool(const Platform::InputEvents::MouseButton&)> m_receiverMouseButton;
        Event::Receiver<bool(const Platform::InputEvents::MouseScroll&)> m_receiverMouseScroll;
        Event::Receiver<bool(const Platform::InputEvents::KeyboardKey&)> m_receiverKeyboardKey;
        Event::Receiver<bool(const Platform::InputEvents::TextInput&)> m_receiverTextInput;

    private:
        Platform::TimerSystem* m_timerSystem = nullptr;
        Platform::WindowSystem* m_windowSystem = nullptr;
        ImGuiContext* m_interface = nullptr;

        EditorSubsystemStorage m_subsystems;
    };
}

REFLECTION_TYPE(Editor::EditorSystem, Core::EngineSystem)
