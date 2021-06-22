/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Core/Service.hpp>

namespace Core
{
    class ServiceStorage;
}

namespace Graphics
{
    class RenderContext;
    class SpriteRenderer;
}

namespace Game
{
    class GameInstance;
}

/*
    Game Renderer
*/

namespace Renderer
{
    class GameRenderer final : public Core::Service
    {
        REFLECTION_ENABLE(GameRenderer, Core::Service)

    public:
        struct DrawParams
        {
            Game::GameInstance* gameInstance = nullptr;
            std::string cameraName = "Camera";
            glm::ivec4 viewportRect = glm::ivec4(0.0f, 0.0f, 0.0f, 0.0f);
            float timeAlpha = 1.0f;
        };

    public:
        GameRenderer();
        ~GameRenderer() override;

        void Draw(const DrawParams& drawParams);

    private:
        bool OnAttach(const Core::ServiceStorage* services) override;

    private:
        Graphics::RenderContext* m_renderContext = nullptr;
        Graphics::SpriteRenderer* m_spriteRenderer = nullptr;
    };
}

REFLECTION_TYPE(Renderer::GameRenderer, Core::Service)
