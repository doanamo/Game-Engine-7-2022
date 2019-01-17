/*
    Copyright (c) 2018-2019 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Precompiled.hpp"

// Forward declarations.
namespace Engine
{
    class Root;
}

namespace Game
{
    class GameState;
}

/*
    State Renderer
*/

namespace Renderer
{
    // State renderer class.
    class StateRenderer
    {
    public:
        // Draw parameters structure.
        struct DrawParams
        {
            DrawParams();

            // Game state reference.
            Game::GameState* gameState;

            // Name of camera entity.
            std::string cameraName;

            // Viewport rectangle in pixels.
            glm::ivec4 viewportRect;

            // Time alpha interpolation.
            float timeAlpha;
        };

    public:
        StateRenderer();
        ~StateRenderer();

        // Disallow copying.
        StateRenderer(const StateRenderer& other) = delete;
        StateRenderer& operator=(const StateRenderer& other) = delete;

        // Move constructor and assignment.
        StateRenderer(StateRenderer&& other);
        StateRenderer& operator=(StateRenderer&& other);

        // Initializes the scene renderer.
        bool Initialize(Engine::Root* engine);

        // Draws a game state.
        void Draw(const DrawParams& drawParams);

    private:
        // Engine reference.
        Engine::Root* m_engine;

        // Initialization state.
        bool m_initialized;
    };
}
