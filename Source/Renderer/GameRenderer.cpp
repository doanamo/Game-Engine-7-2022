/*
    Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Renderer/GameRenderer.hpp"
#include <Core/System/SystemStorage.hpp>
#include <Platform/WindowSystem.hpp>
#include <Platform/Window.hpp>
#include <Graphics/RenderContext.hpp>
#include <Graphics/Sprite/SpriteRenderer.hpp>
#include <Game/Components/TransformComponent.hpp>
#include <Game/Components/CameraComponent.hpp>
#include <Game/Components/SpriteComponent.hpp>
#include <Game/Components/SpriteAnimationComponent.hpp>
#include <Game/GameFramework.hpp>
#include <Game/GameInstance.hpp>
#include <Game/EntitySystem.hpp>
#include <Game/ComponentSystem.hpp>
#include <Game/Systems/IdentitySystem.hpp>
using namespace Renderer;

namespace
{
    const char* LogAttachFailed = "Failed to attach game renderer! {}";
}

GameRenderer::GameRenderer()
{
    m_receivers.drawGameInstance.Bind<GameRenderer, &GameRenderer::OnDrawGameInstance>(this);
}

GameRenderer::~GameRenderer() = default;

bool GameRenderer::OnAttach(const Core::EngineSystemStorage& engineSystems)
{
    // Retrieve needed engine systems.
    m_windowSystem = &engineSystems.Locate<Platform::WindowSystem>();
    m_renderContext = &engineSystems.Locate<Graphics::RenderContext>();
    m_spriteRenderer = &engineSystems.Locate<Graphics::SpriteRenderer>();

    // Subscribe to events.
    auto& gameFramework = engineSystems.Locate<Game::GameFramework>();
    if(!m_receivers.drawGameInstance.Subscribe(gameFramework.events.drawGameInstance))
    {
        LOG_ERROR(LogAttachFailed, "Could not subscribe to game framework events.");
        return false;
    }

    return true;
}

void GameRenderer::OnDrawGameInstance(Game::GameInstance* gameInstance, float timeAlpha)
{
    ASSERT(gameInstance != nullptr);

    const Platform::Window& window = m_windowSystem->GetWindow();

    DrawParams drawParams;
    drawParams.viewportRect = { 0, 0, window.GetWidth(), window.GetHeight() };
    drawParams.gameInstance = gameInstance;
    drawParams.cameraName = "Camera";
    drawParams.timeAlpha = timeAlpha;
    Draw(drawParams);
}

void GameRenderer::Draw(const DrawParams& drawParams)
{
    // Clear frame buffer.
    m_renderContext->GetState().Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Checks if game instance is null.
    if(!drawParams.gameInstance)
    {
        LOG_WARNING("Attempted to draw null game instance!");
        return;
    }

    // Make sure time alpha is normalized.
    ASSERT(drawParams.timeAlpha >= 0.0f && drawParams.timeAlpha <= 1.0f,
        "Time alpha is not normalized!");

    // Retrieve systems from game instance.
    auto& gameSystems = drawParams.gameInstance->GetSystems();
    auto& componentSystem = gameSystems.Locate<Game::ComponentSystem>();
    auto& identitySystem = gameSystems.Locate<Game::IdentitySystem>();

    // Update sprite components for rendering.
    for(auto& spriteAnimationComponent : componentSystem.GetPool<Game::SpriteAnimationComponent>())
    {
        // Update sprite texture view using currently playing animation.
        if(spriteAnimationComponent.IsPlaying())
        {
            Game::SpriteComponent* spriteComponent =
                spriteAnimationComponent.GetSpriteComponent();

            const Game::SpriteAnimationComponent::SpriteAnimation* spriteAnimation =
                spriteAnimationComponent.GetCurrentSpriteAnimation();

            float animationTime = spriteAnimationComponent
                .CalculateAnimationTime(drawParams.timeAlpha);

            ASSERT(spriteAnimation, "Sprite animation is null despite being played!");
            spriteComponent->SetTextureView(
                spriteAnimation->GetFrameByTime(animationTime).textureView);
        }
    }

    // Push render state.
    auto& renderState = m_renderContext->PushState();
    SCOPE_GUARD([this]
    {
        m_renderContext->PopState();
    });

    // Setup drawing viewport.
    renderState.Viewport(
        drawParams.viewportRect.x,
        drawParams.viewportRect.y,
        drawParams.viewportRect.z,
        drawParams.viewportRect.w
    );

    // Base camera transform.
    glm::mat4 cameraTransform(1.0f);

    // Retrieve transform from camera entity.
    auto cameraEntityResult = identitySystem.GetEntityByName(drawParams.cameraName);

    if(cameraEntityResult.IsSuccess())
    {
        auto cameraComponentResult = componentSystem.Lookup<
            Game::CameraComponent>(cameraEntityResult.Unwrap());

        if(cameraComponentResult)
        {
            // Calculate viewport size.
            glm::ivec2 viewportSize;
            viewportSize.x = drawParams.viewportRect.z - drawParams.viewportRect.x;
            viewportSize.y = drawParams.viewportRect.w - drawParams.viewportRect.y;

            // Calculate camera transform.
            cameraTransform = cameraComponentResult.Unwrap()->CalculateTransform(viewportSize);
        }
        else
        {
            LOG_WARNING("Could not retrieve camera component from \"{}\" entity.",
                drawParams.cameraName);
        }
    }
    else
    {
        LOG_WARNING("Could not retrieve \"{}\" camera entity.", drawParams.cameraName);
    }

    // Create list of sprites that will be drawn.
    Graphics::SpriteDrawList spriteDrawList;

    // Iterate all sprite components.
    for(auto& spriteComponent : componentSystem.GetPool<Game::SpriteComponent>())
    {
        // Retrieve transform component.
        Game::TransformComponent* transformComponent = spriteComponent.GetTransformComponent();
        ASSERT(transformComponent != nullptr, "Required transform component is missing!");

        // Add sprite to draw list.
        Graphics::Sprite sprite;
        sprite.info.texture = spriteComponent.GetTextureView().GetTexturePtr();
        sprite.info.transparent = spriteComponent.IsTransparent();
        sprite.info.filtered = spriteComponent.IsFiltered();
        sprite.data.transform = transformComponent->CalculateMatrix(drawParams.timeAlpha);
        sprite.data.rectangle = spriteComponent.GetRectangle();
        sprite.data.coords = spriteComponent.GetTextureView().GetTextureRect();
        sprite.data.color = spriteComponent.GetColor();
        spriteDrawList.AddSprite(sprite);
    }

    // Sort sprite draw list.
    spriteDrawList.SortSprites();

    // Draw sprite components.
    m_spriteRenderer->DrawSprites(spriteDrawList, cameraTransform);
}
