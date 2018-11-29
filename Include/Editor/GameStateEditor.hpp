/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Common/Event/Receiver.hpp"

// Forward declarations.
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
        bool Initialize();

        // Updates the game state editor.
        void Update(float timeDelta);

        // Sets the game state reference.
        void SetGameState(Game::GameState* gameState);

        // Gets the game state reference.
        Game::GameState* GetGameState() const;

    private:
        // Event receivers.
        struct Receivers
        {
            Common::Receiver<void()> gameStateDestructed;
            Common::Receiver<void()> gameStateUpdateCalled;
            Common::Receiver<void(float)> gameStateUpdated;
        } m_receivers;

        // Called when referenced game state gets destructed.
        void OnGameStateDestructed();

        // Called when references game state starts updating.
        void OnGameStateUpdateCalled();

        // Called when referenced game state actually updates.
        void OnGameStateUpdated(float updateTime);

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
