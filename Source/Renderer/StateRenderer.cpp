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

StateRenderer::StateRenderer() = default;
StateRenderer::~StateRenderer() = default;

StateRenderer::CreateResult StateRenderer::Create(const CreateFromParams& params)
{
    LOG("Creating state renderer...");
    LOG_SCOPED_INDENT();

    // Validate arguments
    CHECK_ARGUMENT_OR_RETURN(params.renderContext != nullptr, Common::Failure(CreateErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(params.spriteRenderer != nullptr, Common::Failure(CreateErrors::InvalidArgument));

    // Create instance.
    auto instance = std::unique_ptr<StateRenderer>(new StateRenderer());

    // Save system references.
    instance->m_renderContext = params.renderContext;
    instance->m_spriteRenderer = params.spriteRenderer;

    // Success!
    return Common::Success(std::move(instance));
}

void StateRenderer::Draw(const DrawParams& drawParams)
{
    // Clear frame buffer.
    m_renderContext->GetState().Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Checks if game state is null.
    if(drawParams.gameState == nullptr)
    {
        LOG_WARNING("Attempted to draw null game state!");
        return;
    }

    // Get time alpha from the game state.
    float timeAlpha = drawParams.gameState->updateTimer->GetAlphaTime();
    ASSERT(timeAlpha >= 0.0f && timeAlpha <= 1.0f, "Time alpha is not clamped!");

    // Get game state systems.
    auto& entitySystem = drawParams.gameState->entitySystem;
    auto& componentSystem = drawParams.gameState->componentSystem;
    auto& identitySystem = drawParams.gameState->identitySystem;

    // Update sprite animation components for rendering.
    for(auto& spriteAnimationComponent : componentSystem->GetPool<Game::SpriteAnimationComponent>())
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

    // Push render state.
    auto& renderState = m_renderContext->PushState();
    SCOPE_GUARD(m_renderContext->PopState());

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
    Game::EntityHandle cameraEntity = identitySystem->GetEntityByName(drawParams.cameraName);

    if(entitySystem->IsHandleValid(cameraEntity))
    {
        auto cameraComponent = componentSystem->Lookup<Game::CameraComponent>(cameraEntity);

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

    // Create list of sprites that will be drawn.
    Graphics::SpriteDrawList spriteDrawList;

    // Get all sprite components.
    for(auto& spriteComponent : componentSystem->GetPool<Game::SpriteComponent>())
    {
        // Get transform component.
        Game::TransformComponent* transformComponent = spriteComponent.GetTransformComponent();
        ASSERT(transformComponent != nullptr, "Required transform component is missing!");

        // Add sprite to the draw list.
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

    // Sort sprite draw list.
    spriteDrawList.SortSprites();

    // Draw sprite components.
    m_spriteRenderer->DrawSprites(spriteDrawList, cameraTransform);
}
