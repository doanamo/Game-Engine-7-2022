/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Editor/ModelViewer.hpp"
#include "System/ResourceManager.hpp"
#include "Game/Components/TransformComponent.hpp"
#include "Game/Components/CameraComponent.hpp"
#include "Engine.hpp"
using namespace Editor;

ModelViewer::ModelViewer() :
    m_texture(nullptr),
    m_initialized(false)
{
}

ModelViewer::~ModelViewer()
{
}

ModelViewer::ModelViewer(ModelViewer&& other) :
    ModelViewer()
{
    // Call the move assignment.
    *this = std::move(other);
}

ModelViewer& ModelViewer::operator=(ModelViewer&& other)
{
    // Swap base class.
    Game::BaseScene::operator=(std::move(other));

    // Swap class members.
    std::swap(m_texture, other.m_texture);
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool ModelViewer::Initialize(Engine::Root* engine)
{
    LOG() << "Initializing model viewer..." << LOG_INDENT();

    // Make sure instance is not initialized.
    VERIFY(!m_initialized, "Model viewer instance has already been initialized!");

    // Initialize the base class.
    if(!Game::BaseScene::Initialize(engine))
    {
        LOG_ERROR() << "Could not initialize the base game scene!";
        return false;
    }

    // Reset class instance if initialization fails.
    SCOPE_GUARD_IF(!m_initialized, *this = ModelViewer());

    // Load the texture.
    Graphics::Texture::LoadFromFile textureParams;
    textureParams.engine = engine;
    textureParams.filePath = "Data/Engine/Textures/Checker.png";

    m_texture = GetEngine()->resourceManager.Acquire<Graphics::Texture>(
        textureParams.filePath, textureParams);

    // Create camera entity.
    {
        Game::EntityHandle cameraEntity = entitySystem.CreateEntity();
        identitySystem.SetEntityName(cameraEntity, "Camera");

        auto* transform = componentSystem.Create<Game::Components::Transform>(cameraEntity);
        ASSERT(transform != nullptr, "Could not create a transform component!");

        transform->position = glm::vec3(0.0f, 0.0f, 4.0f);
        transform->rotation = glm::vec3(0.0f, 0.0f, 0.0f);

        auto* camera = componentSystem.Create<Game::Components::Camera>(cameraEntity);
        ASSERT(camera != nullptr, "Could not create a camera component!");

        camera->SetupPerspective(90.0f, 0.1f, 1000.0f);
    }

    // Success!
    return m_initialized = true;
}

void ModelViewer::OnUpdate(float timeDelta)
{
    ASSERT(m_initialized, "Model viever instance has not been initialized!");

    // Call the base scene update.
    Game::BaseScene::OnUpdate(timeDelta);
}

void ModelViewer::OnDraw(const Game::SceneDrawParams& drawParams)
{
    ASSERT(m_initialized, "Model viever instance has not been initialized!");

    // Call the base on draw method.
    Game::BaseScene::OnDraw(drawParams);

    // Get the current render state.
    auto& renderState = GetEngine()->renderContext.GetState();

    // Retrieve camera entity component.
    Game::EntityHandle cameraEntity = identitySystem.GetEntityByName("Camera");

    auto camera = componentSystem.Lookup<Game::Components::Camera>(cameraEntity);
    ASSERT(camera != nullptr, "Could not retrieve camera component!");

    // Calculate combined view and projection matrix.
    glm::mat4 cameraTransform = camera->CalculateTransform(drawParams.CalculateViewportSize());

    // Draw the texture.
    Graphics::Sprite sprite;
    sprite.info.texture = m_texture.get();
    sprite.info.transparent = false;
    sprite.info.filtered = true;
    sprite.data.transform = glm::mat4(1.0f);
    sprite.data.rectangle = glm::vec4(-0.5f, -0.5f, 0.5f, 0.5f);
    sprite.data.coords = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    sprite.data.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    Graphics::SpriteList spriteList;
    spriteList.AddSprite(sprite);

    GetEngine()->spriteRenderer.DrawSprites(spriteList, cameraTransform);
}
