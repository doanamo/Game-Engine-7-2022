/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <Event/Receiver.hpp>

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
    class GameStateEditor : private NonCopyable
    {
    public:
        struct InitializeFromParams
        {
            Game::GameFramework* gameFramework = nullptr;
        };

    public:
        GameStateEditor();
        ~GameStateEditor();

        GameStateEditor(GameStateEditor&& other);
        GameStateEditor& operator=(GameStateEditor&& other);

        bool Initialize(const InitializeFromParams& params);
        void Update(float timeDelta);

        Game::GameState* GetGameState() const;

    public:
        bool mainWindowOpen = false;

    private:
        struct Receivers
        {
            Event::Receiver<void(const std::shared_ptr<Game::GameState>&)> gameStateChanged;
            Event::Receiver<void()> gameStateDestructed;
            Event::Receiver<void()> gameStateUpdateCalled;
            Event::Receiver<void(float)> gameStateUpdateProcessed;
        } m_receivers;

        void OnGameStateChanged(const std::shared_ptr<Game::GameState>& gameState);
        void OnGameStateDestructed();
        void OnGameStateUpdateCalled();
        void OnGameStateUpdateProcessed(float updateTime);

    private:
        Game::GameState* m_gameState = nullptr;

        std::vector<float> m_updateTimeHistogram;
        bool m_updateTimeHistogramPaused = false;

        float m_updateRateSlider = 0.0f;
        float m_updateDelaySlider = 0.0f;
        float m_updateDelayValue = 0.0f;
        float m_updateNoiseSlider = 0.0f;
        float m_updateNoiseValue = 0.0f;
        float m_updateFreezeSlider = 1.0f;

        bool m_initialized = false;
    };
}
