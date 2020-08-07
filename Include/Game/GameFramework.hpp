/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <Core/ServiceStorage.hpp>
#include <Event/Dispatcher.hpp>

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
    class GameInstance;

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

        void SetGameInstance(std::shared_ptr<GameInstance> gameInstance);
        std::shared_ptr<GameInstance> GetGameInstance() const;

        struct Events
        {
            Event::Dispatcher<void(const std::shared_ptr<GameInstance>&)> gameInstanceChanged;
        } events;

    private:
        GameFramework();

        System::Timer* m_timer = nullptr;
        System::Window* m_window = nullptr;
        Renderer::GameRenderer* m_gameRenderer = nullptr;

        std::shared_ptr<GameInstance> m_gameInstance;
    };
}
