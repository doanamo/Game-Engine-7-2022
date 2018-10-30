/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Game/SceneRenderer.hpp"
#include "Game/BaseScene.hpp"
#include "Game/Components/TransformComponent.hpp"
#include "Game/Components/CameraComponent.hpp"
#include "Game/Components/SpriteComponent.hpp"
#include "Engine.hpp"
using namespace Game;

SceneRenderer::SceneRenderer() :
    m_engine(nullptr),
    m_initialized(false)
{
}

SceneRenderer::~SceneRenderer()
{
}

SceneRenderer::SceneRenderer(SceneRenderer&& other)
{
    // Call the assignment operator.
    *this = std::move(other);
}

SceneRenderer& SceneRenderer::operator=(SceneRenderer&& other)
{
    // Swap class members.
    std::swap(m_engine, other.m_engine);
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool SceneRenderer::Initialize(Engine::Root* engine)
{
    LOG() << "Initializing scene renderer..." << LOG_INDENT();

    // Make sure class instance has not been already initialized.
    ASSERT(!m_initialized, "Scene renderer has already been initialized!");

    // Reset class instance if initialization fails.
    SCOPE_GUARD_IF(!m_initialized, *this = SceneRenderer());

    // Validate and save engine reference.
    if(engine == nullptr)
    {
        LOG_ERROR() << "Invalid argument - \"engine\" is nullptr!";
        return false;
    }

    m_engine = engine;

    // Success!
    return m_initialized = true;
}

void SceneRenderer::DrawScene(Scene* scene, const SceneDrawParams& drawParams)
{
    ASSERT(m_initialized, "Scene renderer has not been initialized yet!");
 
    // Make sure the scene pointer is valid.
    if(scene == nullptr)
        return;

    // Push the render state.
    auto& renderState = m_engine->renderContext.PushState();
    SCOPE_GUARD(m_engine->renderContext.PopState());

    // Setup the drawing viewport.
    renderState.Viewport(
        drawParams.viewportRect.x,
        drawParams.viewportRect.y,
        drawParams.viewportRect.z,
        drawParams.viewportRect.w
    );

    // Check if current scene is a base scene (one that has an entity system).
    BaseScene* baseScene = dynamic_cast<BaseScene*>(scene);

    if(baseScene)
    {
        // Get entity system references.
        auto& entitySystem = baseScene->entitySystem;
        auto& componentSystem = baseScene->componentSystem;
        auto& identitySystem = baseScene->identitySystem;

        // Base camera transform.
        glm::mat4 cameraTransform(1.0f);

        // Retrieve transform from camera entity.
        Game::EntityHandle cameraEntity = identitySystem.GetEntityByName(drawParams.cameraName);

        if(entitySystem.IsHandleValid(cameraEntity))
        {
            auto cameraComponent = componentSystem.Lookup<CameraComponent>(cameraEntity);

            if(cameraComponent != nullptr)
            {
                // Calculate the camera transform.
                glm::ivec2 viewportSize = drawParams.CalculateViewportSize();
                cameraTransform = cameraComponent->CalculateTransform(viewportSize);
            }
            else
            {
                LOG_WARNING() << "Could not retrieve camera component from \"" << drawParams.cameraName
                    << "\" entity in \"" << baseScene->GetDebugName() << "\" scene.";
            }
        }
        else
        {
            LOG_WARNING() << "Could not retrieve \"" << drawParams.cameraName
                << "\" camera entity from \"" << baseScene->GetDebugName() << "\" scene.";
        }
        
        // Create a list of sprites that will be drawn.
        Graphics::SpriteDrawList spriteDrawList;

        // Get all sprite components.
        for(auto& spriteComponent : componentSystem.GetPool<SpriteComponent>())
        {
            // Get the transform component.
            TransformComponent* transformComponent = spriteComponent.GetTransformComponent();
            ASSERT(transformComponent != nullptr, "Required transform component is missing!");

            // Add a sprite to the draw list.
            Graphics::Sprite sprite;
            sprite.info.texture = spriteComponent.GetTextureView().GetTexturePtr();
            sprite.info.transparent = spriteComponent.IsTransparent();
            sprite.info.filtered = spriteComponent.IsFiltered();
            sprite.data.transform = transformComponent->CalculateMatrix();
            sprite.data.rectangle = spriteComponent.GetRectangle();
            sprite.data.coords = spriteComponent.GetTextureView().GetTextureRect();
            sprite.data.color = spriteComponent.GetColor();

            spriteDrawList.AddSprite(sprite);
        }

        // Sort the sprite draw list.
        spriteDrawList.SortSprites();

        // Draw sprite components.
        m_engine->spriteRenderer.DrawSprites(spriteDrawList, cameraTransform);
    }

    // Call the drawing method.
    scene->OnDraw(drawParams);
}
