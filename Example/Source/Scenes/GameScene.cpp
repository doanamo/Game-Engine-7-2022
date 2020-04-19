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
    m_customUpdate.Bind<GameScene, &GameScene::Update>(this);
}

GameScene::~GameScene() = default;

GameScene::CreateResult GameScene::Create(Engine::Root* engine)
{
    LOG("Creating game scene...");
    LOG_SCOPED_INDENT();

    // Validate engine reference.
    CHECK_ARGUMENT_OR_RETURN(engine != nullptr, Failure(CreateErrors::InvalidArgument));

    // Create instance.
    auto instance = std::unique_ptr<GameScene>(new GameScene());

    // Create game state.
    instance->m_gameState = Game::GameState::Create().UnwrapOr(nullptr);
    if(instance->m_gameState == nullptr)
    {
        LOG_ERROR("Could not create game state!");
        return Failure(CreateErrors::FailedGameStateCreation);
    }

    // Setup custom update callback.
    instance->m_customUpdate.Subscribe(instance->m_gameState->events.updateProcessed);

    // Set game state as current.
    engine->GetGameFramework().SetGameState(instance->m_gameState);

    // Load sprite animation list.
    Graphics::SpriteAnimationList::LoadFromFile spriteAnimationListParams;
    spriteAnimationListParams.fileSystem = &engine->GetFileSystem();
    spriteAnimationListParams.resourceManager = &engine->GetResourceManager();
    spriteAnimationListParams.renderContext = &engine->GetRenderContext();
    spriteAnimationListParams.filePath = "Data/Engine/Textures/Checker.animation";

    auto spriteAnimationList = engine->GetResourceManager().Acquire<Graphics::SpriteAnimationList>(
        spriteAnimationListParams.filePath, spriteAnimationListParams).UnwrapOr(nullptr);

    if(spriteAnimationList == nullptr)
    {
        LOG_ERROR("Could not load sprite animation list!");
        return Failure(CreateErrors::FailedResourceLoading);
    }

    // Load texture atlas.
    Graphics::TextureAtlas::LoadFromFile textureAtlasParams;
    textureAtlasParams.fileSystem = &engine->GetFileSystem();
    textureAtlasParams.resourceManager = &engine->GetResourceManager();
    textureAtlasParams.filePath = "Data/Engine/Textures/Checker.atlas";

    auto textureAtlas = engine->GetResourceManager().Acquire<Graphics::TextureAtlas>(
        textureAtlasParams.filePath, textureAtlasParams).UnwrapOr(nullptr);

    if(textureAtlas == nullptr)
    {
        LOG_ERROR("Could not load texture atlas!");
        return Failure(CreateErrors::FailedResourceLoading);
    }

    // Create camera entity.
    {
        // Create named entity.
        Game::EntityHandle cameraEntity = instance->m_gameState->entitySystem->CreateEntity();
        instance->m_gameState->identitySystem->SetEntityName(cameraEntity, "Camera");

        // Create transform component.
        auto* transform = instance->m_gameState->componentSystem->Create<Game::TransformComponent>(cameraEntity);
        ASSERT(transform != nullptr, "Could not create a transform component!");

        transform->SetPosition(glm::vec3(0.0f, 0.0f, 2.0f));

        // Create camera component.
        auto* camera = instance->m_gameState->componentSystem->Create<Game::CameraComponent>(cameraEntity);
        ASSERT(camera != nullptr, "Could not create a camera component!");

        camera->SetupOrthogonal(glm::vec2(16.0f, 9.0f), 0.1f, 1000.0f);
    }

    // Create player entity.
    {
        // Load texture.
        Graphics::Texture::LoadFromFile textureParams;
        textureParams.fileSystem = &engine->GetFileSystem();
        textureParams.renderContext = &engine->GetRenderContext();
        textureParams.filePath = "Data/Engine/Textures/Checker.png";

        Graphics::TexturePtr texture = engine->GetResourceManager().Acquire<Graphics::Texture>(
            textureParams.filePath, textureParams).UnwrapEither();

        // Create named entity.
        Game::EntityHandle playerEntity = instance->m_gameState->entitySystem->CreateEntity();
        instance->m_gameState->identitySystem->SetEntityName(playerEntity, "Player");

        // Create transform component.
        auto* transform = instance->m_gameState->componentSystem->Create<Game::TransformComponent>(playerEntity);

        transform->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));

        // Create sprite component.
        auto* sprite = instance->m_gameState->componentSystem->Create<Game::SpriteComponent>(playerEntity);

        sprite->SetRectangle(glm::vec4(-0.5f, -0.5f, 0.5f, 0.5f));
        sprite->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        sprite->SetTextureView(textureAtlas->GetRegion("animation_frame_3"));
        sprite->SetTransparent(false);
        sprite->SetFiltered(true);

        // Create sprite animation component.
        auto* spriteAnimation = instance->m_gameState->componentSystem->Create<Game::SpriteAnimationComponent>(playerEntity);

        spriteAnimation->SetSpriteAnimationList(spriteAnimationList);
        spriteAnimation->Play("rotation", true);
    }

    // Save engine reference.
    instance->m_engine = engine;

    // Success!
    return Success(std::move(instance));
}

void GameScene::Update(float updateTime)
{
    // Retrieve player transform.
    Game::EntityHandle playerEntity = m_gameState->identitySystem->GetEntityByName("Player");

    auto transform = m_gameState->componentSystem->Lookup<Game::TransformComponent>(playerEntity);
    ASSERT(transform != nullptr, "Could not create a transform component!");

    // Animate the entity.
    double timeAccumulated = m_gameState->updateTimer->GetTotalUpdateTime();

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
