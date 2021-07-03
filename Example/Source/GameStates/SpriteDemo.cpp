/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "../Precompiled.hpp"
#include "SpriteDemo.hpp"
#include <Engine.hpp>
#include <System/Timer.hpp>
#include <System/InputManager.hpp>
#include <System/ResourceManager.hpp>
#include <Graphics/TextureAtlas.hpp>
#include <Graphics/Sprite/SpriteAnimationList.hpp>
#include <Game/TickTimer.hpp>
#include <Game/GameInstance.hpp>
#include <Game/GameFramework.hpp>
#include <Game/EntitySystem.hpp>
#include <Game/ComponentSystem.hpp>
#include <Game/Systems/IdentitySystem.hpp>
#include <Game/Components/TransformComponent.hpp>
#include <Game/Components/CameraComponent.hpp>
#include <Game/Components/SpriteComponent.hpp>
#include <Game/Components/SpriteAnimationComponent.hpp>
#include <Editor/EditorSystem.hpp>

SpriteDemo::SpriteDemo() = default;
SpriteDemo::~SpriteDemo() = default;

SpriteDemo::CreateResult SpriteDemo::Create(Engine::Root* engine)
{
    LOG("Creating game scene...");
    LOG_SCOPED_INDENT();

    // Validate engine reference.
    CHECK_ARGUMENT_OR_RETURN(engine != nullptr, Common::Failure(CreateErrors::InvalidArgument));

    // Acquire engine systems.
    auto* inputManager = engine->GetSystems().Locate<System::InputManager>();
    auto* resourceManager = engine->GetSystems().Locate<System::ResourceManager>();
    auto* gameFramework = engine->GetSystems().Locate<Game::GameFramework>();

    // Create instance.
    auto instance = std::unique_ptr<SpriteDemo>(new SpriteDemo());

    // Create tick timer.
    instance->m_tickTimer = Game::TickTimer::Create().UnwrapOr(nullptr);
    if(instance->m_tickTimer == nullptr)
    {
        LOG_ERROR("Could not create tick timer!");
        return Common::Failure(CreateErrors::FailedTickTimerCreate);
    }

    // Create game instance.
    instance->m_gameInstance = Game::GameInstance::Create().UnwrapOr(nullptr);
    if(instance->m_gameInstance == nullptr)
    {
        LOG_ERROR("Could not create game instance!");
        return Common::Failure(CreateErrors::FailedGameInstanceCreate);
    }

    // Load sprite animation list.
    Graphics::SpriteAnimationList::LoadFromFile spriteAnimationListParams;
    spriteAnimationListParams.engineSystems = &engine->GetSystems();

    auto spriteAnimationList = resourceManager->Acquire<Graphics::SpriteAnimationList>(
        "Data/Textures/Checker.animation", spriteAnimationListParams).UnwrapOr(nullptr);

    if(spriteAnimationList == nullptr)
    {
        LOG_ERROR("Could not load sprite animation list!");
        return Common::Failure(CreateErrors::FailedResourceLoad);
    }

    // Load texture atlas.
    Graphics::TextureAtlas::LoadFromFile textureAtlasParams;
    textureAtlasParams.engineSystems = &engine->GetSystems();

    auto textureAtlas = resourceManager->Acquire<Graphics::TextureAtlas>(
        "Data/Textures/Checker.atlas", textureAtlasParams).UnwrapOr(nullptr);

    if(textureAtlas == nullptr)
    {
        LOG_ERROR("Could not load texture atlas!");
        return Common::Failure(CreateErrors::FailedResourceLoad);
    }

    // Retrieve game systems.
    auto* entitySystem = instance->m_gameInstance->GetSystems().Locate<Game::EntitySystem>();
    auto* componentSystem = instance->m_gameInstance->GetSystems().Locate<Game::ComponentSystem>();
    auto* identitySystem = instance->m_gameInstance->GetSystems().Locate<Game::IdentitySystem>();
    ASSERT(entitySystem && componentSystem && identitySystem);

    // Create camera entity.
    {
        // Create named entity.
        Game::EntityHandle cameraEntity = entitySystem->CreateEntity();
        identitySystem->SetEntityName(cameraEntity, "Camera");

        // Create transform component.
        auto* transform = componentSystem->Create<Game::TransformComponent>(cameraEntity);
        ASSERT(transform != nullptr, "Could not create transform component!");
        transform->SetPosition(glm::vec3(0.0f, 0.0f, 2.0f));

        // Create camera component.
        auto* camera = componentSystem->Create<Game::CameraComponent>(cameraEntity);
        ASSERT(camera != nullptr, "Could not create camera component!");
        camera->SetupOrthogonal(glm::vec2(16.0f, 9.0f), 0.1f, 1000.0f);
    }

    // Create player entity.
    {
        // Create named entity.
        Game::EntityHandle playerEntity = entitySystem->CreateEntity();
        identitySystem->SetEntityName(playerEntity, "Player");

        // Create transform component.
        auto* transform = componentSystem->Create<Game::TransformComponent>(playerEntity);
        transform->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
        transform->SetScale(glm::vec3(1.0f) * (2.0f + (float)glm::cos(0.0f)));
        transform->SetRotation(glm::rotate(glm::identity<glm::quat>(),
            2.0f * glm::pi<float>() * ((float)std::fmod(0.0f, 10.0) / 10.0f),
            glm::vec3(0.0f, 0.0f, 1.0f)));

        // Create sprite component.
        auto* sprite = componentSystem->Create<Game::SpriteComponent>(playerEntity);
        sprite->SetRectangle(glm::vec4(-0.5f, -0.5f, 0.5f, 0.5f));
        sprite->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        sprite->SetTextureView(textureAtlas->GetRegion("animation_frame_3"));
        sprite->SetTransparent(false);
        sprite->SetFiltered(true);

        // Create sprite animation component.
        auto* spriteAnimation =
            componentSystem->Create<Game::SpriteAnimationComponent>(playerEntity);
        spriteAnimation->SetSpriteAnimationList(spriteAnimationList);
        spriteAnimation->Play("rotation", true);
    }

    // Save engine reference.
    instance->m_engine = engine;

    // Success!
    return Common::Success(std::move(instance));
}

void SpriteDemo::Tick(const float tickTime)
{
    // Retrieve game systems.
    auto* componentSystem = m_gameInstance->GetSystems().Locate<Game::ComponentSystem>();
    auto* identitySystem = m_gameInstance->GetSystems().Locate<Game::IdentitySystem>();

    // Retrieve player transform.
    Game::EntityHandle playerEntity = identitySystem->GetEntityByName("Player").Unwrap();
    auto transform = componentSystem->Lookup<Game::TransformComponent>(playerEntity);
    ASSERT(transform != nullptr, "Could not create transform component!");

    // Animate the entity.
    double timeAccumulated = m_tickTimer->GetTotalTickSeconds();
    transform->SetScale(glm::vec3(1.0f) * (2.0f + (float)glm::cos(timeAccumulated)));
    transform->SetRotation(glm::rotate(glm::identity<glm::quat>(),
        2.0f * glm::pi<float>() * ((float)std::fmod(timeAccumulated, 10.0) / 10.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)));

    // Control the entity with keyboard.
    auto* inputManager = m_engine->GetSystems().Locate<System::InputManager>();
    System::InputState& inputState = inputManager->GetInputState();

    glm::vec3 direction(0.0f, 0.0f, 0.0f);

    if(inputState.IsKeyboardKeyPressed(System::KeyboardKeys::KeyLeft))
    {
        direction.x -= 1.0f;
    }

    if(inputState.IsKeyboardKeyPressed(System::KeyboardKeys::KeyRight))
    {
        direction.x += 1.0f;
    }

    if(inputState.IsKeyboardKeyPressed(System::KeyboardKeys::KeyUp))
    {
        direction.y += 1.0f;
    }

    if(inputState.IsKeyboardKeyPressed(System::KeyboardKeys::KeyDown))
    {
        direction.y -= 1.0f;
    }

    if(direction != glm::vec3(0.0f))
    {
        transform->SetPosition(transform->GetPosition()
            + 4.0f * glm::normalize(direction) * tickTime);
    }
}

void SpriteDemo::Update(const float timeDelta)
{
}

void SpriteDemo::Draw(const float timeAlpha)
{
}

Game::TickTimer* SpriteDemo::GetTickTimer() const
{
    return m_tickTimer.get();
}

Game::GameInstance* SpriteDemo::GetGameInstance() const
{
    return m_gameInstance.get();
}
