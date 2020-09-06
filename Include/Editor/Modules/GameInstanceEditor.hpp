/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <Event/Receiver.hpp>

namespace Core
{
    class ServiceStorage;
}

namespace Game
{
    class TickTimer;
    class GameFramework;
    class GameInstance;
    class GameState;
};

/*
    Game Instance Editor

    Editor interface for manipulating game instance while it is running.
*/

namespace Editor
{
    class GameInstanceEditor final : private Common::NonCopyable
    {
    public:
        struct CreateFromParams
        {
            const Core::ServiceStorage* services = nullptr;
        };

        enum class CreateErrors
        {
            InvalidArgument,
            FailedEventSubscription,
        };

        using CreateResult = Common::Result<std::unique_ptr<GameInstanceEditor>, CreateErrors>;
        static CreateResult Create(const CreateFromParams& params);

        ~GameInstanceEditor();

        void Display(float timeDelta);

        bool mainWindowOpen = false;

    private:
        GameInstanceEditor();

        bool SubscribeEvents(const Core::ServiceStorage* services);
        void OnGameStateChanged(const std::shared_ptr<Game::GameState>& gameState);
        void OnTickRequested();
        void OnTickProcessed(float tickTime);

        struct Receivers
        {
            Event::Receiver<void(const std::shared_ptr<Game::GameState>&)> gameStateChanged;
            Event::Receiver<void()> tickRequested;
            Event::Receiver<void(float)> tickProcessed;
        } m_receivers;

        Game::TickTimer* m_tickTimer = nullptr;
        Game::GameInstance* m_gameInstance = nullptr;

        std::vector<float> m_tickTimeHistogram;
        bool m_tickTimeHistogramPaused = false;

        float m_tickRateSlider = 0.0f;
        float m_updateDelaySlider = 0.0f;
        float m_updateDelayValue = 0.0f;
        float m_updateNoiseSlider = 0.0f;
        float m_updateNoiseValue = 0.0f;
        float m_updateFreezeSlider = 1.0f;
    };
}
