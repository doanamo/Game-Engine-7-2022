/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <string>

// Forward declarations.
namespace Graphics
{
    class RenderContext;
    class SpriteRenderer;
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
    class StateRenderer : private NonCopyable
    {
    public:
        // Initialization parameters.
        struct InitializeFromParams
        {
            Graphics::RenderContext* renderContext = nullptr;
            Graphics::SpriteRenderer* spriteRenderer = nullptr;
        };

        // Draw parameters.
        struct DrawParams
        {
            // Game state reference.
            Game::GameState* gameState = nullptr;

            // Name of camera entity.
            std::string cameraName = "Camera";

            // Viewport rectangle in pixels.
            glm::ivec4 viewportRect = glm::ivec4(0.0f, 0.0f, 0.0f, 0.0f);

            // Time alpha interpolation.
            float timeAlpha = 1.0f;
        };

    public:
        StateRenderer() = default;
        ~StateRenderer() = default;

        // Move constructor and assignment.
        StateRenderer(StateRenderer&& other);
        StateRenderer& operator=(StateRenderer&& other);

        // Initializes the scene renderer.
        bool Initialize(const InitializeFromParams& params);

        // Draws a game state.
        void Draw(const DrawParams& drawParams);

    private:
        // System references.
        Graphics::RenderContext* m_renderContext = nullptr;
        Graphics::SpriteRenderer* m_spriteRenderer = nullptr;

        // Initialization state.
        bool m_initialized = false;
    };
}
