/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Event/Receiver.hpp"

// Forward declarations.
namespace Engine
{
    class Root;
};

namespace Game
{
    class GameState;
};

/*
    Game State Editor
*/

namespace Editor
{
    // Game state editor class.
    class GameStateEditor
    {
    public:
        GameStateEditor();
        ~GameStateEditor();

        GameStateEditor(const GameStateEditor& other) = delete;
        GameStateEditor& operator=(const GameStateEditor& other) = delete;

        GameStateEditor(GameStateEditor&& other);
        GameStateEditor& operator=(GameStateEditor&& other);

        // Initializes the game state editor.
        bool Initialize(Engine::Root* engine);

        // Updates the game state editor.
        void Update(float timeDelta);

        // Gets the game state reference.
        Game::GameState* GetGameState() const;

    public:
        // Window state.
        bool mainWindowOpen;

    private:
        // Event receivers.
        struct Receivers
        {
            Event::Receiver<void(const std::shared_ptr<Game::GameState>&)> gameStateChanged;
            Event::Receiver<void()> gameStateDestructed;
            Event::Receiver<void()> gameStateUpdateCalled;
            Event::Receiver<void(float)> gameStateUpdateProcessed;
        } m_receivers;

        // Changed when the current game state changes.
        void OnGameStateChanged(const std::shared_ptr<Game::GameState>& gameState);

        // Called when referenced game state gets destructed.
        void OnGameStateDestructed();

        // Called when referenced game state starts updating.
        void OnGameStateUpdateCalled();

        // Called when referenced game state actually updates.
        void OnGameStateUpdateProcessed(float updateTime);

    private:
        // Game state reference.
        Game::GameState* m_gameState;

        // Update rate slider value.
        float m_updateRateSlider;

        // Update time histogram.
        std::vector<float> m_updateTimeHistogram;
        bool m_updateTimeHistogramPaused;

        // Update delay slider value.
        float m_updateDelaySlider;
        float m_updateDelayValue;

        // Update noise slider value.
        float m_updateNoiseSlider;
        float m_updateNoiseValue;

        // Update freeze slider value.
        float m_updateFreezeSlider;

        // Initialization state.
        bool m_initialized;
    };
}
