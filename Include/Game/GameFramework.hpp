/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <Core/ServiceStorage.hpp>
#include "Game/EventRouter.hpp"

namespace System
{
    class Timer;
    class Window;
    class InputManager;
}

namespace Renderer
{
    class StateRenderer;
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
            FailedEventRouterCreation,
        };

        using CreateResult = Common::Result<std::unique_ptr<GameFramework>, CreateErrors>;
        static CreateResult Create(const CreateFromParams& params);

    public:
        ~GameFramework();

        bool Update();
        void Draw();

        void SetGameState(std::shared_ptr<GameState> gameState);
        std::shared_ptr<GameState> GetGameState() const;

    public:
        struct Events
        {
            Event::Dispatcher<void(const std::shared_ptr<GameState>&)> gameStateChanged;
        } events;

    private:
        GameFramework();

    private:
        System::Timer* m_timer = nullptr;
        System::Window* m_window = nullptr;
        Renderer::StateRenderer* m_stateRenderer = nullptr;

        std::unique_ptr<EventRouter> m_eventRouter;
        std::shared_ptr<GameState> m_gameState;
    };
}
