/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Core/Service.hpp>
#include <Common/Event/Receiver.hpp>
#include <System/InputDefinitions.hpp>

namespace Core
{
    class ServiceStorage;
}

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
    class EditorSystem final : public Core::Service
    {
        REFLECTION_ENABLE(EditorSystem, Core::Service)

    public:
        enum class CreateErrors
        {
        };

        using CreateResult = Common::Result<std::unique_ptr<EditorSystem>, CreateErrors>;
        static CreateResult Create();

    public:
        ~EditorSystem() override;

        void BeginInterface(float timeDelta);
        void EndInterface();

    private:
        EditorSystem();

        bool OnAttach(const Core::ServiceStorage* serviceStorage) override;

        bool CreateContext();
        bool CreateSubsystems(const Core::ServiceStorage* services);
        bool SubscribeEvents(const Core::ServiceStorage* services);

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

REFLECTION_TYPE(Editor::EditorSystem, Core::Service)
