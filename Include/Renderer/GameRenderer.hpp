/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Common/Event/Receiver.hpp>
#include <Core/EngineSystem.hpp>

namespace System
{
    class Window;
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
    class GameRenderer final : public Core::EngineSystem
    {
        REFLECTION_ENABLE(GameRenderer, Core::EngineSystem)

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
        bool OnAttach(const Core::EngineSystemStorage& engineSystems) override;
        void OnDrawGameInstance(Game::GameInstance* gameInstance, float timeAlpha);

        struct Receivers
        {
            Event::Receiver<void(Game::GameInstance*, float)> drawGameInstance;
        } m_receivers;

    private:
        System::Window* m_window = nullptr;
        Graphics::RenderContext* m_renderContext = nullptr;
        Graphics::SpriteRenderer* m_spriteRenderer = nullptr;
    };
}

REFLECTION_TYPE(Renderer::GameRenderer, Core::EngineSystem)
