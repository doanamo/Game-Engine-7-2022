/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <Core/ServiceStorage.hpp>
#include <Event/Dispatcher.hpp>
#include "Game/GameState.hpp"

namespace System
{
    class Timer;
    class Window;
    class InputManager;
}

namespace Renderer
{
    class GameRenderer;
}

/*
    Game Framework
*/

namespace Game
{
    class GameState;

    class GameFramework final : private Common::NonCopyable
    {
    public:
        struct CreateFromParams
        {
            const Core::ServiceStorage* services = nullptr;
        };

        enum class CreateErrors
        {
            InvalidArgument,
        };

        using CreateResult = Common::Result<std::unique_ptr<GameFramework>, CreateErrors>;
        static CreateResult Create(const CreateFromParams& params);

    public:
        ~GameFramework();

        bool ChangeGameState(std::shared_ptr<GameState> gameState);
        bool ProcessGameState(float timeDelta);
        bool HasGameState() const;

        struct Events
        {
            Event::Dispatcher<void(const std::shared_ptr<GameState>&)> gameStateChanged;
        } events;

    private:
        GameFramework();

        System::Timer* m_timer = nullptr;
        System::Window* m_window = nullptr;
        Renderer::GameRenderer* m_gameRenderer = nullptr;

        Common::StateMachine<GameState> m_stateMachine;
    };
}
