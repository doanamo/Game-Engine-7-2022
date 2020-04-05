/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "GameScene.hpp"
#include <Engine.hpp>
#include <System/Timer.hpp>
#include <System/InputManager.hpp>
#include <System/ResourceManager.hpp>
#include <Graphics/TextureAtlas.hpp>
#include <Graphics/Sprite/SpriteAnimationList.hpp>
#include <Game/GameFramework.hpp>
#include <Game/Components/TransformComponent.hpp>
#include <Game/Components/CameraComponent.hpp>
#include <Game/Components/SpriteComponent.hpp>
#include <Game/Components/SpriteAnimationComponent.hpp>
#include <Editor/EditorSystem.hpp>

GameScene::GameScene()
{
    // Bind event receivers.
    m_customUpdate.Bind<GameScene, &GameScene::Update>(this);
}

GameScene::~GameScene()
{
}

GameScene::GameScene(GameScene&& other) :
    GameScene()
{
    *this = std::move(other);
}

GameScene& GameScene::operator=(GameScene&& other)
{
    std::swap(m_engine, other.m_engine);
    std::swap(m_gameState, other.m_gameState);
    std::swap(m_customUpdate, other.m_customUpdate);
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool GameScene::Initialize(Engine::Root* engine)
{
    LOG("Initializing game scene...");
    LOG_SCOPED_INDENT();

    // Make sure instance has not been initialized.
    VERIFY(!m_initialized, "Game scene has already been initialized!");

    // Reset class instance on failed initialization.
    SCOPE_GUARD_IF(!m_initialized, *this = GameScene());

    // Validate engine reference.
    if(engine == nullptr && engine->IsInitialized())
    {
        LOG_ERROR("Invalid argument - \"engine\" is invalid!");
        return false;
    }

    m_engine = engine;

    // Initialize game state.
    m_gameState = std::make_shared<Game::GameState>();
    if(!m_gameState->Initialize())
    {
        LOG_ERROR("Could not initialize game state!");
        return false;
    }

    // Setup custom update callback.
    m_customUpdate.Subscribe(m_gameState->events.updateProcessed);

    // Set game state as current.
    m_engine->GetGameFramework().SetGameState(m_gameState);

    // Load sprite animation list.
    Graphics::SpriteAnimationList::LoadFromFile spriteAnimationListParams;
    spriteAnimationListParams.fileSystem = &m_engine->GetFileSystem();
    spriteAnimationListParams.resourceManager = &m_engine->GetResourceManager();
    spriteAnimationListParams.renderContext = &m_engine->GetRenderContext();
    spriteAnimationListParams.filePath = "Data/Engine/Textures/Checker.animation";

    auto spriteAnimationList = m_engine->GetResourceManager().Acquire<Graphics::SpriteAnimationList>(
        spriteAnimationListParams.filePath, spriteAnimationListParams);

    if(!spriteAnimationList)
    {
        LOG_ERROR("Could not load sprite animation list!");
        return false;
    }

    // Load texture atlas.
    Graphics::TextureAtlas::LoadFromFile textureAtlasParams;
    textureAtlasParams.fileSystem = &m_engine->GetFileSystem();
    textureAtlasParams.resourceManager = &m_engine->GetResourceManager();
    textureAtlasParams.filePath = "Data/Engine/Textures/Checker.atlas";

    auto textureAtlas = m_engine->GetResourceManager().Acquire<Graphics::TextureAtlas>(
        textureAtlasParams.filePath, textureAtlasParams);

    if(!spriteAnimationList)
    {
        LOG_ERROR("Could not load texture atlas!");
        return false;
    }

    // Create camera entity.
    {
        // Create named entity.
        Game::EntityHandle cameraEntity = m_gameState->entitySystem.CreateEntity();
        m_gameState->identitySystem.SetEntityName(cameraEntity, "Camera");

        // Create transform component.
        auto* transform = m_gameState->componentSystem.Create<Game::TransformComponent>(cameraEntity);
        ASSERT(transform != nullptr, "Could not create a transform component!");

        transform->SetPosition(glm::vec3(0.0f, 0.0f, 2.0f));

        // Create camera component.
        auto* camera = m_gameState->componentSystem.Create<Game::CameraComponent>(cameraEntity);
        ASSERT(camera != nullptr, "Could not create a camera component!");

        camera->SetupOrthogonal(glm::vec2(16.0f, 9.0f), 0.1f, 1000.0f);
    }

    // Create player entity.
    {
        // Load texture.
        Graphics::Texture::LoadFromFile textureParams;
        textureParams.fileSystem = &m_engine->GetFileSystem();
        textureParams.renderContext = &m_engine->GetRenderContext();
        textureParams.filePath = "Data/Engine/Textures/Checker.png";

        Graphics::TexturePtr texture = m_engine->GetResourceManager().Acquire<Graphics::Texture>(
            textureParams.filePath, textureParams);

        // Create named entity.
        Game::EntityHandle playerEntity = m_gameState->entitySystem.CreateEntity();
        m_gameState->identitySystem.SetEntityName(playerEntity, "Player");

        // Create transform component.
        auto* transform = m_gameState->componentSystem.Create<Game::TransformComponent>(playerEntity);

        transform->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));

        // Create sprite component.
        auto* sprite = m_gameState->componentSystem.Create<Game::SpriteComponent>(playerEntity);

        sprite->SetRectangle(glm::vec4(-0.5f, -0.5f, 0.5f, 0.5f));
        sprite->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        sprite->SetTextureView(textureAtlas->GetRegion("animation_frame_3"));
        sprite->SetTransparent(false);
        sprite->SetFiltered(true);

        // Create sprite animation component.
        auto* spriteAnimation = m_gameState->componentSystem.Create<Game::SpriteAnimationComponent>(playerEntity);

        spriteAnimation->SetSpriteAnimationList(spriteAnimationList);
        spriteAnimation->Play("rotation", true);
    }

    // Success!
    return m_initialized = true;
}

void GameScene::Update(float updateTime)
{
    ASSERT(m_initialized, "Main scene has not been initialized!");

    // Retrieve player transform.
    Game::EntityHandle playerEntity = m_gameState->identitySystem.GetEntityByName("Player");

    auto transform = m_gameState->componentSystem.Lookup<Game::TransformComponent>(playerEntity);
    ASSERT(transform != nullptr, "Could not create a transform component!");

    // Animate the entity.
    double timeAccumulated = m_gameState->updateTimer.GetTotalUpdateTime();

    transform->SetScale(glm::vec3(1.0f) * (2.0f + (float)glm::cos(timeAccumulated)));
    transform->SetRotation(glm::rotate(glm::identity<glm::quat>(), 2.0f * glm::pi<float>() * ((float)std::fmod(timeAccumulated, 10.0) / 10.0f), glm::vec3(0.0f, 0.0f, 1.0f)));

    // Control the entity with keyboard.
    glm::vec3 direction(0.0f, 0.0f, 0.0f);

    if(m_engine->GetInputManager().IsKeyboardKeyPressed(System::KeyboardKeys::KeyLeft))
    {
        direction.x -= 1.0f;
    }

    if(m_engine->GetInputManager().IsKeyboardKeyPressed(System::KeyboardKeys::KeyRight))
    {
        direction.x += 1.0f;
    }

    if(m_engine->GetInputManager().IsKeyboardKeyPressed(System::KeyboardKeys::KeyUp))
    {
        direction.y += 1.0f;
    }

    if(m_engine->GetInputManager().IsKeyboardKeyPressed(System::KeyboardKeys::KeyDown))
    {
        direction.y -= 1.0f;
    }

    if(direction != glm::vec3(0.0f))
    {
        transform->SetPosition(transform->GetPosition() + 4.0f * glm::normalize(direction) * updateTime);
    }
}
