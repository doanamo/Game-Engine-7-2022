/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Scenes/GameScene.hpp"
#include <System/ResourceManager.hpp>
#include <Graphics/TextureAtlas.hpp>
#include <Graphics/Sprites/SpriteAnimationList.hpp>
#include <Game/Components/TransformComponent.hpp>
#include <Game/Components/CameraComponent.hpp>
#include <Game/Components/SpriteComponent.hpp>
#include <Game/Components/SpriteAnimationComponent.hpp>

GameScene::GameScene() :
    m_initialized(false)
{
}

GameScene::~GameScene()
{
}

GameScene::GameScene(GameScene&& other) :
    GameScene()
{
    // Call the move assignment.
    *this = std::move(other);
}

GameScene& GameScene::operator=(GameScene&& other)
{
    // Swap base class.
    Game::BaseScene::operator=(std::move(other));

    // Swap class members.
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool GameScene::Initialize(Engine::Root* engine)
{
    LOG() << "Initializing main scene..." << LOG_INDENT();

    // Make sure instance has not been initialized.
    VERIFY(!m_initialized, "Main scene has already been initialized!");

    // Initialize the base class.
    if(!Game::BaseScene::Initialize(engine))
    {
        LOG_ERROR() << "Could not initialize the base game scene!";
        return false;
    }

    // Reset class instance on failed initialization.
    SCOPE_GUARD_IF(!m_initialized, *this = GameScene());

    // Success!
    return m_initialized = true;
}

void GameScene::OnEnter()
{
    ASSERT(m_initialized, "Main scene has not been initialized!");

    // Load sprite animation list.
    Graphics::SpriteAnimationList::LoadFromFile spriteAnimationListParams;
    spriteAnimationListParams.engine = this->GetEngine();
    spriteAnimationListParams.filePath = "Data/Engine/Textures/Checker.animation";

    auto spriteAnimationList = this->GetEngine()->resourceManager.Acquire<Graphics::SpriteAnimationList>(
        spriteAnimationListParams.filePath, spriteAnimationListParams);

    // Load texture atlas.
    Graphics::TextureAtlas::LoadFromFile textureAtlasParams;
    textureAtlasParams.engine = this->GetEngine();
    textureAtlasParams.filePath = "Data/Engine/Textures/Checker.atlas";

    auto textureAtlas = this->GetEngine()->resourceManager.Acquire<Graphics::TextureAtlas>(
        textureAtlasParams.filePath, textureAtlasParams);

    // Create camera entity.
    {
        // Create named entity.
        Game::EntityHandle cameraEntity = entitySystem.CreateEntity();
        identitySystem.SetEntityName(cameraEntity, "Camera");

        // Create transform component.
        auto* transform = componentSystem.Create<Game::TransformComponent>(cameraEntity);
        ASSERT(transform != nullptr, "Could not create a transform component!");

        transform->position = glm::vec3(0.0f, 0.0f, 2.0f);
        transform->rotation = glm::vec3(0.0f, 0.0f, 0.0f);

        // Create camera component.
        auto* camera = componentSystem.Create<Game::CameraComponent>(cameraEntity);
        ASSERT(camera != nullptr, "Could not create a camera component!");

        camera->SetupOrthogonal(glm::vec2(16.0f, 9.0f), 0.1f, 1000.0f);
    }

    // Create player entity.
    {
        // Load texture.
        Graphics::Texture::LoadFromFile textureParams;
        textureParams.engine = GetEngine();
        textureParams.filePath = "Data/Engine/Textures/Checker.png";

        Graphics::TexturePtr texture = GetEngine()->resourceManager.Acquire<Graphics::Texture>(
            textureParams.filePath, textureParams);

        // Create named entity.
        Game::EntityHandle playerEntity = entitySystem.CreateEntity();
        identitySystem.SetEntityName(playerEntity, "Player");

        // Create transform component.
        auto* transform = componentSystem.Create<Game::TransformComponent>(playerEntity);

        transform->position = glm::vec3(0.0f, 0.0f, 0.0f);
        transform->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        transform->scale = glm::vec3(1.0f, 1.0f, 1.0f);

        // Create sprite component.
        auto* sprite = componentSystem.Create<Game::SpriteComponent>(playerEntity);
        
        sprite->SetRectangle(glm::vec4(-0.5f, -0.5f, 0.5f, 0.5f));
        sprite->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        sprite->SetTextureView(textureAtlas->GetRegion("animation_frame_3"));
        sprite->SetTransparent(false);
        sprite->SetFiltered(true);

        // Create sprite animation component.
        auto* spriteAnimation = componentSystem.Create<Game::SpriteAnimationComponent>(playerEntity);

        spriteAnimation->SetSpriteAnimationList(spriteAnimationList);
        spriteAnimation->Play("rotation", true);
    }
}

void GameScene::OnUpdate(float timeDelta)
{
    ASSERT(m_initialized, "Main scene has not been initialized!");

    // Update the base class.
    Game::BaseScene::OnUpdate(timeDelta);

    // Retrieve player transform.
    Game::EntityHandle playerEntity = identitySystem.GetEntityByName("Player");

    auto transform = componentSystem.Lookup<Game::TransformComponent>(playerEntity);
    ASSERT(transform != nullptr, "Could not create a transform component!");

    // Animate the entity.
    transform->scale = glm::vec3(1.0f, 1.0f, 1.0f) *
        (2.0f + (float)glm::cos(GetEngine()->timer.GetCurrentTime()));

    // Control the entity with keyboard.
    glm::vec3 direction(0.0f, 0.0f, 0.0f);

    if(GetEngine()->inputState.IsKeyboardKeyDown(GLFW_KEY_A))
    {
        direction.x -= 1.0f;
    }

    if(GetEngine()->inputState.IsKeyboardKeyDown(GLFW_KEY_D))
    {
        direction.x += 1.0f;
    }

    if(GetEngine()->inputState.IsKeyboardKeyDown(GLFW_KEY_W))
    {
        direction.y += 1.0f;
    }

    if(GetEngine()->inputState.IsKeyboardKeyDown(GLFW_KEY_S))
    {
        direction.y -= 1.0f;
    }

    if(direction != glm::vec3(0.0f))
    {
        transform->position += 4.0f * glm::normalize(direction) * timeDelta;
    }
}

void GameScene::OnDraw(const Game::SceneDrawParams& drawParams)
{
    ASSERT(m_initialized, "Game scene has not been initialized!");

    // Call the base class.
    Game::BaseScene::OnDraw(drawParams);
}

void GameScene::OnExit()
{
    ASSERT(m_initialized, "Main scene has not been initialized!");
}
