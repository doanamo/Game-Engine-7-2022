/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "../Precompiled.hpp"
#include "SpriteDemo.hpp"
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

SpriteDemo::SpriteDemo()
{
    m_customUpdate.Bind<SpriteDemo, &SpriteDemo::Update>(this);
}

SpriteDemo::~SpriteDemo() = default;

SpriteDemo::CreateResult SpriteDemo::Create(Engine::Root* engine)
{
    LOG("Creating game scene...");
    LOG_SCOPED_INDENT();

    // Validate engine reference.
    CHECK_ARGUMENT_OR_RETURN(engine != nullptr, Common::Failure(CreateErrors::InvalidArgument));

    // Acquire engine services.
    System::ResourceManager* resourceManager = engine->GetServices().GetResourceManager();
    Game::GameFramework* gameFramework = engine->GetServices().GetGameFramework();

    // Create instance.
    auto instance = std::unique_ptr<SpriteDemo>(new SpriteDemo());

    // Create game state.
    instance->m_gameState = Game::GameState::Create().UnwrapOr(nullptr);
    if(instance->m_gameState == nullptr)
    {
        LOG_ERROR("Could not create game state!");
        return Common::Failure(CreateErrors::FailedGameStateCreation);
    }

    // Setup custom update callback.
    instance->m_customUpdate.Subscribe(instance->m_gameState->events.updateProcessed);

    // Set game state as current.
    gameFramework->SetGameState(instance->m_gameState);

    // Load sprite animation list.
    Graphics::SpriteAnimationList::LoadFromFile spriteAnimationListParams;
    spriteAnimationListParams.services = &engine->GetServices();

    auto spriteAnimationList = resourceManager->Acquire<Graphics::SpriteAnimationList>(
        "Data/Textures/Checker.animation", spriteAnimationListParams).UnwrapOr(nullptr);

    if(spriteAnimationList == nullptr)
    {
        LOG_ERROR("Could not load sprite animation list!");
        return Common::Failure(CreateErrors::FailedResourceLoading);
    }

    // Load texture atlas.
    Graphics::TextureAtlas::LoadFromFile textureAtlasParams;
    textureAtlasParams.services = &engine->GetServices();

    auto textureAtlas = resourceManager->Acquire<Graphics::TextureAtlas>(
        "Data/Textures/Checker.atlas", textureAtlasParams).UnwrapOr(nullptr);

    if(textureAtlas == nullptr)
    {
        LOG_ERROR("Could not load texture atlas!");
        return Common::Failure(CreateErrors::FailedResourceLoading);
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
    return Common::Success(std::move(instance));
}

void SpriteDemo::Update(float updateTime)
{
    // Retrieve player transform.
    Game::EntityHandle playerEntity = m_gameState->identitySystem->GetEntityByName("Player");

    auto transform = m_gameState->componentSystem->Lookup<Game::TransformComponent>(playerEntity);
    ASSERT(transform != nullptr, "Could not create a transform component!");

    // Animate the entity.
    double timeAccumulated = m_gameState->updateTimer->GetTotalUpdateSeconds();

    transform->SetScale(glm::vec3(1.0f) * (2.0f + (float)glm::cos(timeAccumulated)));
    transform->SetRotation(glm::rotate(glm::identity<glm::quat>(), 2.0f * glm::pi<float>() * ((float)std::fmod(timeAccumulated, 10.0) / 10.0f), glm::vec3(0.0f, 0.0f, 1.0f)));

    // Acquire input manager service.
    System::InputManager* inputManager = m_engine->GetServices().GetInputManager();

    // Control the entity with keyboard.
    glm::vec3 direction(0.0f, 0.0f, 0.0f);

    if(inputManager->IsKeyboardKeyPressed(System::KeyboardKeys::KeyLeft))
    {
        direction.x -= 1.0f;
    }

    if(inputManager->IsKeyboardKeyPressed(System::KeyboardKeys::KeyRight))
    {
        direction.x += 1.0f;
    }

    if(inputManager->IsKeyboardKeyPressed(System::KeyboardKeys::KeyUp))
    {
        direction.y += 1.0f;
    }

    if(inputManager->IsKeyboardKeyPressed(System::KeyboardKeys::KeyDown))
    {
        direction.y -= 1.0f;
    }

    if(direction != glm::vec3(0.0f))
    {
        transform->SetPosition(transform->GetPosition() + 4.0f * glm::normalize(direction) * updateTime);
    }
}
