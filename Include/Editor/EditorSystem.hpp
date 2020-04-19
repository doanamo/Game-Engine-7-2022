/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <Event/Receiver.hpp>
#include <System/InputDefinitions.hpp>
#include "Editor/EditorRenderer.hpp"
#include "Editor/EditorShell.hpp"

namespace System
{
    class FileSystem;
    class ResourceManager;
    class InputManager;
    class Window;
};

namespace Graphics
{
    class RenderContext;
}

namespace Game
{
    class GameFramework;
};

/*
    Editor System

    Displays and handles different editor interfaces.
*/

namespace Editor
{
    class EditorSystem final : private NonCopyable
    {
    public:
        struct CreateFromParams
        {
            System::FileSystem* fileSystem = nullptr;
            System::ResourceManager* resourceManager = nullptr;
            System::InputManager* inputManager = nullptr;
            System::Window* window = nullptr;
            Graphics::RenderContext* renderContext = nullptr;
            Game::GameFramework* gameFramework = nullptr;
        };

        enum class CreateErrors
        {
            InvalidArgument,
            FailedContextCreation,
            FailedEventSubscription,
            FailedSubsystemInitialization,
        };

        using CreateResult = Result<std::unique_ptr<EditorSystem>, CreateErrors>;
        static CreateResult Create(const CreateFromParams& params);

    public:
        ~EditorSystem();

        void Update(float timeDelta);
        void Draw();

    private:
        EditorSystem();

        Event::Receiver<void(const System::InputEvents::CursorPosition&)> m_receiverCursorPosition;
        Event::Receiver<bool(const System::InputEvents::MouseButton&)> m_receiverMouseButton;
        Event::Receiver<bool(const System::InputEvents::MouseScroll&)> m_receiverMouseScroll;
        Event::Receiver<bool(const System::InputEvents::KeyboardKey&)> m_receiverKeyboardKey;
        Event::Receiver<bool(const System::InputEvents::TextInput&)> m_receiverTextInput;

        void CursorPositionCallback(const System::InputEvents::CursorPosition& event);
        bool MouseButtonCallback(const System::InputEvents::MouseButton& event);
        bool MouseScrollCallback(const System::InputEvents::MouseScroll& event);
        bool KeyboardKeyCallback(const System::InputEvents::KeyboardKey& event);
        bool TextInputCallback(const System::InputEvents::TextInput& event);

    private:
        ImGuiContext* m_interface = nullptr;

        std::unique_ptr<EditorRenderer> m_editorRenderer;
        std::unique_ptr<EditorShell> m_editorShell;
    };
}
