/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
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
    class GameInstance;
}

/*
    Game Renderer
*/

namespace Renderer
{
    class GameRenderer final : private Common::NonCopyable
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

        using CreateResult = Common::Result<std::unique_ptr<GameRenderer>, CreateErrors>;
        static CreateResult Create(const CreateFromParams& params);

        struct DrawParams
        {
            Game::GameInstance* gameInstance = nullptr;
            std::string cameraName = "Camera";
            glm::ivec4 viewportRect = glm::ivec4(0.0f, 0.0f, 0.0f, 0.0f);
            float timeAlpha = 1.0f;
        };

    public:
        ~GameRenderer();

        void Draw(const DrawParams& drawParams);

    private:
        GameRenderer();

        Graphics::RenderContext* m_renderContext = nullptr;
        Graphics::SpriteRenderer* m_spriteRenderer = nullptr;
    };
}
