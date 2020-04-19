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
    class StateRenderer final : private NonCopyable
    {
    public:
        enum class CreateErrors
        {
            InvalidArgument,
        };

        struct CreateFromParams
        {
            Graphics::RenderContext* renderContext = nullptr;
            Graphics::SpriteRenderer* spriteRenderer = nullptr;
        };

        using CreateResult = Result<std::unique_ptr<StateRenderer>, CreateErrors>;
        static CreateResult Create(const CreateFromParams& params);

        struct DrawParams
        {
            Game::GameState* gameState = nullptr;
            std::string cameraName = "Camera";
            glm::ivec4 viewportRect = glm::ivec4(0.0f, 0.0f, 0.0f, 0.0f);
            float timeAlpha = 1.0f;
        };

    public:
        ~StateRenderer();

        void Draw(const DrawParams& drawParams);

    private:
        StateRenderer();

    private:
        Graphics::RenderContext* m_renderContext = nullptr;
        Graphics::SpriteRenderer* m_spriteRenderer = nullptr;
    };
}
