/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Game/Precompiled.hpp"
#include "Game/GameInstance.hpp"
#include "Game/EntitySystem.hpp"
#include "Game/ComponentSystem.hpp"
#include "Game/Systems/IdentitySystem.hpp"
#include "Game/Systems/InterpolationSystem.hpp"
#include "Game/Systems/SpriteSystem.hpp"
using namespace Game;

namespace
{
    const char* LogCreateSystemsFailed = "Failed to create game systems! {}";
}

GameInstance::GameInstance() = default;
GameInstance::~GameInstance() = default;

GameInstance::CreateResult GameInstance::Create()
{
    LOG("Creating game instance...");
    LOG_SCOPED_INDENT();

    auto instance = std::unique_ptr<GameInstance>(new GameInstance());

    // Create  default game engine systems.
    const std::vector<Reflection::TypeIdentifier> defaultGameSystemTypes =
    {
        Reflection::GetIdentifier<EntitySystem>(),
        Reflection::GetIdentifier<ComponentSystem>(),
        Reflection::GetIdentifier<IdentitySystem>(),
        Reflection::GetIdentifier<InterpolationSystem>(),
        Reflection::GetIdentifier<SpriteSystem>(),
    };

    if(!instance->m_gameSystems.CreateFromTypes(defaultGameSystemTypes))
    {
        LOG_ERROR(LogCreateSystemsFailed, "Could not populate system storage.");
        return Common::Failure(CreateErrors::FailedGameSystemCreation);
    }

    if(!instance->m_gameSystems.Finalize())
    {
        LOG_ERROR(LogCreateSystemsFailed, "Could not finalize system storage.");
        return Common::Failure(CreateErrors::FailedGameSystemCreation);
    }

    return Common::Success(std::move(instance));
}

void GameInstance::Tick(const float timeDelta)
{
    m_gameSystems.ForEach([timeDelta](GameSystem& gameSystem)
    {
        gameSystem.OnTick(timeDelta);
        return true;
    });
}
