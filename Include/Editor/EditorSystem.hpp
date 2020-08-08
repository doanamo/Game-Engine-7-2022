/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <Event/Receiver.hpp>
#include <Core/ServiceStorage.hpp>
#include <System/InputDefinitions.hpp>
#include "Editor/EditorRenderer.hpp"
#include "Editor/EditorShell.hpp"

namespace System
{
    class InputState;
    class Window;
};

/*
    Editor System

    Displays and handles different editor interfaces.
*/

namespace Editor
{
    class EditorSystem final : private Common::NonCopyable
    {
    public:
        struct CreateFromParams
        {
            const Core::ServiceStorage* services = nullptr;
        };

        enum class CreateErrors
        {
            InvalidArgument,
            FailedContextCreation,
            FailedEventSubscription,
            FailedSubsystemCreation,
        };

        using CreateResult = Common::Result<std::unique_ptr<EditorSystem>, CreateErrors>;
        static CreateResult Create(const CreateFromParams& params);

    public:
        ~EditorSystem();

        void Update(float timeDelta);
        void Draw();

    private:
        EditorSystem();

        void OnInputStateChanged(System::InputState* inputState);
        bool OnTextInput(const System::InputEvents::TextInput& event);
        bool OnKeyboardKey(const System::InputEvents::KeyboardKey& event);
        bool OnMouseButton(const System::InputEvents::MouseButton& event);
        bool OnMouseScroll(const System::InputEvents::MouseScroll& event);
        void OnCursorPosition(const System::InputEvents::CursorPosition& event);

        Event::Receiver<void(System::InputState*)> m_receiverInputStateChanged;
        Event::Receiver<void(const System::InputEvents::CursorPosition&)> m_receiverCursorPosition;
        Event::Receiver<bool(const System::InputEvents::MouseButton&)> m_receiverMouseButton;
        Event::Receiver<bool(const System::InputEvents::MouseScroll&)> m_receiverMouseScroll;
        Event::Receiver<bool(const System::InputEvents::KeyboardKey&)> m_receiverKeyboardKey;
        Event::Receiver<bool(const System::InputEvents::TextInput&)> m_receiverTextInput;

        System::Window* m_window = nullptr;
        ImGuiContext* m_interface = nullptr;

        std::unique_ptr<EditorRenderer> m_editorRenderer;
        std::unique_ptr<EditorShell> m_editorShell;
    };
}
