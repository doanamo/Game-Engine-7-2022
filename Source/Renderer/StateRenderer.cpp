/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Renderer/StateRenderer.hpp"
#include <Graphics/RenderContext.hpp>
#include <Graphics/Sprite/SpriteRenderer.hpp>
#include <Game/Components/TransformComponent.hpp>
#include <Game/Components/CameraComponent.hpp>
#include <Game/Components/SpriteComponent.hpp>
#include <Game/Components/SpriteAnimationComponent.hpp>
#include <Game/GameState.hpp>
using namespace Renderer;

StateRenderer::StateRenderer(StateRenderer&& other) :
    StateRenderer()
{
    *this = std::move(other);
}

StateRenderer& StateRenderer::operator=(StateRenderer&& other)
{
    std::swap(m_renderContext, other.m_renderContext);
    std::swap(m_spriteRenderer, other.m_spriteRenderer);
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool StateRenderer::Initialize(const InitializeFromParams& params)
{
    LOG("Initializing state renderer...");
    LOG_SCOPED_INDENT();

    // Make sure class instance has not been already initialized.
    ASSERT(!m_initialized, "State renderer has already been initialized!");

    // Reset class instance if initialization fails.
    SCOPE_GUARD_IF(!m_initialized, *this = StateRenderer());

    // Validate arguments
    if(params.renderContext == nullptr)
    {
        LOG_ERROR("Invalid argument - \"renderContext\" is nullptr!");
        return false;
    }

    if(params.spriteRenderer == nullptr)
    {
        LOG_ERROR("Invalid argument - \"spriteRenderer\" is nullptr!");
        return false;
    }

    m_renderContext = params.renderContext;
    m_spriteRenderer = params.spriteRenderer;

    // Success!
    return m_initialized = true;
}

void StateRenderer::Draw(const DrawParams& drawParams)
{
    ASSERT(m_initialized, "State renderer has not been initialized yet!");

    // Clear the frame buffer.
    m_renderContext->GetState().Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Checks if game state is null.
    if(drawParams.gameState == nullptr)
    {
        LOG_WARNING("Attempted to draw null game state!");
        return;
    }

    // Get time alpha from the game state.
    float timeAlpha = drawParams.gameState->updateTimer.GetAlphaTime();
    ASSERT(timeAlpha >= 0.0f && timeAlpha <= 1.0f, "Time alpha is not clamped!");

    // Get game state systems.
    auto& entitySystem = drawParams.gameState->entitySystem;
    auto& componentSystem = drawParams.gameState->componentSystem;
    auto& identitySystem = drawParams.gameState->identitySystem;

    // Update sprite animation components for rendering.
    for(auto& spriteAnimationComponent : componentSystem.GetPool<Game::SpriteAnimationComponent>())
    {
        // Update sprite texture view using currently playing animation.
        if(spriteAnimationComponent.IsPlaying())
        {
            Game::SpriteComponent* spriteComponent = spriteAnimationComponent.GetSpriteComponent();

            auto spriteAnimation = spriteAnimationComponent.GetSpriteAnimation();
            float animationTime = spriteAnimationComponent.CalculateAnimationTime(timeAlpha);

            ASSERT(spriteAnimation, "Sprite animation is null despite being played!");
            spriteComponent->SetTextureView(spriteAnimation->GetFrameByTime(animationTime).textureView);
        }
    }

    // Push the render state.
    auto& renderState = m_renderContext->PushState();
    SCOPE_GUARD(m_renderContext->PopState());

    // Setup the drawing viewport.
    renderState.Viewport(
        drawParams.viewportRect.x,
        drawParams.viewportRect.y,
        drawParams.viewportRect.z,
        drawParams.viewportRect.w
    );

    // Base camera transform.
    glm::mat4 cameraTransform(1.0f);

    // Retrieve transform from camera entity.
    Game::EntityHandle cameraEntity = identitySystem.GetEntityByName(drawParams.cameraName);

    if(entitySystem.IsHandleValid(cameraEntity))
    {
        auto cameraComponent = componentSystem.Lookup<Game::CameraComponent>(cameraEntity);

        if(cameraComponent != nullptr)
        {
            // Calculate viewport size.
            glm::ivec2 viewportSize;
            viewportSize.x = drawParams.viewportRect.z - drawParams.viewportRect.x;
            viewportSize.y = drawParams.viewportRect.w - drawParams.viewportRect.y;

            // Calculate camera transform.
            cameraTransform = cameraComponent->CalculateTransform(viewportSize);
        }
        else
        {
            LOG_WARNING("Could not retrieve camera component from \"{}\" entity.", drawParams.cameraName);
        }
    }
    else
    {
        LOG_WARNING("Could not retrieve \"{}\" camera entity.", drawParams.cameraName);
    }

    // Create a list of sprites that will be drawn.
    Graphics::SpriteDrawList spriteDrawList;

    // Get all sprite components.
    for(auto& spriteComponent : componentSystem.GetPool<Game::SpriteComponent>())
    {
        // Get the transform component.
        Game::TransformComponent* transformComponent = spriteComponent.GetTransformComponent();
        ASSERT(transformComponent != nullptr, "Required transform component is missing!");

        // Add a sprite to the draw list.
        Graphics::Sprite sprite;
        sprite.info.texture = spriteComponent.GetTextureView().GetTexturePtr();
        sprite.info.transparent = spriteComponent.IsTransparent();
        sprite.info.filtered = spriteComponent.IsFiltered();
        sprite.data.transform = transformComponent->CalculateMatrix(timeAlpha);
        sprite.data.rectangle = spriteComponent.GetRectangle();
        sprite.data.coords = spriteComponent.GetTextureView().GetTextureRect();
        sprite.data.color = spriteComponent.GetColor();

        spriteDrawList.AddSprite(sprite);
    }

    // Sort the sprite draw list.
    spriteDrawList.SortSprites();

    // Draw sprite components.
    m_spriteRenderer->DrawSprites(spriteDrawList, cameraTransform);
}
