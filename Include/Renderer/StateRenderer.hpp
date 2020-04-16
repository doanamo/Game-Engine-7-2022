/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <string>

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
    class StateRenderer final : private NonCopyable, public Resettable<StateRenderer>
    {
    public:
        enum class InitializeErrors
        {
            InvalidArgument,
        };

        using InitializeResult = Result<void, InitializeErrors>;

        struct InitializeFromParams
        {
            Graphics::RenderContext* renderContext = nullptr;
            Graphics::SpriteRenderer* spriteRenderer = nullptr;
        };

        struct DrawParams
        {
            Game::GameState* gameState = nullptr;
            std::string cameraName = "Camera";
            glm::ivec4 viewportRect = glm::ivec4(0.0f, 0.0f, 0.0f, 0.0f);
            float timeAlpha = 1.0f;
        };

    public:
        StateRenderer();
        ~StateRenderer();

        InitializeResult Initialize(const InitializeFromParams& params);
        void Draw(const DrawParams& drawParams);

    private:
        Graphics::RenderContext* m_renderContext = nullptr;
        Graphics::SpriteRenderer* m_spriteRenderer = nullptr;
        bool m_initialized = false;
    };
}
