/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Editor/Modules/InputManagerEditor.hpp"
#include "Editor/Modules/GameStateEditor.hpp"

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
*/

namespace Editor
{
    class EditorShell final : private Common::NonCopyable
    {
    public:
        struct CreateFromParams
        {
            System::Window* window = nullptr;
            Game::GameFramework* gameFramework = nullptr;
        };

        enum class CreateErrors
        {
            InvalidArgument,
            FailedModuleCreation,
        };

        using CreateResult = Common::Result<std::unique_ptr<EditorShell>, CreateErrors>;
        static CreateResult Create(const CreateFromParams& params);

    public:
        ~EditorShell();

        void Update(float timeDelta);

    private:
        EditorShell();

    private:
        System::Window* m_window = nullptr;

        std::unique_ptr<InputManagerEditor> m_inputManagerEditor;
        std::unique_ptr<GameStateEditor> m_gameStateEditor;

        bool m_showDemoWindow = false;
    };
}
