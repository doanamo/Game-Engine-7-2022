/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Core/EngineSystem.hpp>
#include <Common/Event/Receiver.hpp>
#include <System/InputDefinitions.hpp>

namespace System
{
    class InputState;
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

        void BeginInterface(float timeDelta);
        void EndInterface();

    private:
        bool OnAttach(const Core::EngineSystemStorage& engineSystems) override;

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
        System::Window* m_window = nullptr;
        ImGuiContext* m_interface = nullptr;

        std::unique_ptr<EditorRenderer> m_editorRenderer;
        std::unique_ptr<EditorConsole> m_editorConsole;
        std::unique_ptr<EditorShell> m_editorShell;
    };
}

REFLECTION_TYPE(Editor::EditorSystem, Core::EngineSystem)
