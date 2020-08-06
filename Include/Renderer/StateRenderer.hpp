/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <string>
#include <Core/ServiceStorage.hpp>

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
    class StateRenderer final : private Common::NonCopyable
    {
    public:
        enum class CreateErrors
        {
            InvalidArgument,
        };

        struct CreateFromParams
        {
            const Core::ServiceStorage* services = nullptr;
        };

        using CreateResult = Common::Result<std::unique_ptr<StateRenderer>, CreateErrors>;
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

        Graphics::RenderContext* m_renderContext = nullptr;
        Graphics::SpriteRenderer* m_spriteRenderer = nullptr;
    };
}
