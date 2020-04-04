/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <Event/Receiver.hpp>

// Forward declarations.
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
    // Game state editor class.
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

        // Initializes the game state editor.
        bool Initialize(const InitializeFromParams& params);

        // Updates the game state editor.
        void Update(float timeDelta);

        // Gets the game state reference.
        Game::GameState* GetGameState() const;

    public:
        // Window state.
        bool mainWindowOpen = false;

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
        Game::GameState* m_gameState = nullptr;

        // Update rate slider value.
        float m_updateRateSlider = 0.0f;

        // Update time histogram.
        std::vector<float> m_updateTimeHistogram;
        bool m_updateTimeHistogramPaused = false;

        // Update delay slider value.
        float m_updateDelaySlider = 0.0f;
        float m_updateDelayValue = 0.0f;

        // Update noise slider value.
        float m_updateNoiseSlider = 0.0f;
        float m_updateNoiseValue = 0.0f;

        // Update freeze slider value.
        float m_updateFreezeSlider = 1.0f;

        // Initialization state.
        bool m_initialized = false;
    };
}
