/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <Event/Receiver.hpp>
#include <Core/ServiceStorage.hpp>

namespace Game
{
    class GameFramework;
    class GameState;
};

/*
    Game State Editor
*/

namespace Editor
{
    class GameStateEditor final : private Common::NonCopyable
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

        using CreateResult = Common::Result<std::unique_ptr<GameStateEditor>, CreateErrors>;
        static CreateResult Create(const CreateFromParams& params);

    public:
        ~GameStateEditor();

        void Update(float timeDelta);
        Game::GameState* GetGameState() const;

    public:
        bool mainWindowOpen = false;

    private:
        GameStateEditor();

        struct Receivers
        {
            Event::Receiver<void(const std::shared_ptr<Game::GameState>&)> gameStateChanged;
            Event::Receiver<void()> gameStateDestructed;
            Event::Receiver<void()> gameStateTickCalled;
            Event::Receiver<void(float)> gameStateTickProcessed;
        } m_receivers;

        void OnGameStateChanged(const std::shared_ptr<Game::GameState>& gameState);
        void OnGameStateDestructed();
        void OnGameStateTickCalled();
        void OnGameStateTickProcessed(float tickTime);

    private:
        Game::GameState* m_gameState = nullptr;

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
