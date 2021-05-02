/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Game/Precompiled.hpp"
#include "Game/GameInstance.hpp"
#include "Game/GameSystem.hpp"
#include "Game/EntitySystem.hpp"
#include "Game/ComponentSystem.hpp"
#include "Game/Systems/IdentitySystem.hpp"
#include "Game/Systems/InterpolationSystem.hpp"
#include "Game/Systems/SpriteSystem.hpp"
using namespace Game;

GameInstance::GameInstance() = default;
GameInstance::~GameInstance()
{
    for(auto it = m_gameSystemList.rbegin(); it != m_gameSystemList.rend(); ++it)
    {
        it->reset();
    }
}

GameInstance::CreateResult GameInstance::Create()
{
    LOG("Creating game instance...");
    LOG_SCOPED_INDENT();

    auto instance = std::unique_ptr<GameInstance>(new GameInstance());

    Reflection::TypeIdentifier defaultGameSystemTypes[] =
    {
        Reflection::GetIdentifier<EntitySystem>(),
        Reflection::GetIdentifier<ComponentSystem>(),
        Reflection::GetIdentifier<IdentitySystem>(),
        Reflection::GetIdentifier<InterpolationSystem>(),
        Reflection::GetIdentifier<SpriteSystem>(),
    };

    for(auto& gameSystemType : defaultGameSystemTypes)
    {
        GameSystemPtr gameSystem(Reflection::Construct<GameSystem>(gameSystemType));

        if(gameSystem != nullptr)
        {
            LOG_INFO("Created \"{}\" game system.",
                Reflection::GetName(gameSystemType).GetString());

            if(!instance->AttachSystem(gameSystem))
            {
                LOG_ERROR("Could not attach default game system \"{}\"!",
                    Reflection::GetName(gameSystemType).GetString());
                return Common::Failure(CreateErrors::FailedGameSystemAttach);
            }
        }
        else
        {
            LOG_ERROR("Could not create default game system \"{}\"!",
                Reflection::GetName(gameSystemType).GetString());
            return Common::Failure(CreateErrors::FailedGameSystemCreation);
        }
    }

    return Common::Success(std::move(instance));
}

GameInstance::AttachSystemResult GameInstance::AttachSystem(std::unique_ptr<GameSystem>& gameSystem)
{
    if(gameSystem == nullptr)
    {
        LOG_WARNING("Attempted to attach null game system to game instance!");
        return Common::Failure(AttachSystemErrors::NullInstance);
    }

    const Reflection::TypeIdentifier gameSystemType = Reflection::GetIdentifier(gameSystem);
    if(m_gameSystemMap.find(gameSystemType) != m_gameSystemMap.end())
    {
        LOG_ERROR("Game system \"{}\" already exists in game instance!",
            Reflection::GetName(gameSystem).GetString());
        return Common::Failure(AttachSystemErrors::AlreadyExists);
    }

    if(!gameSystem->OnAttach(this))
    {
        LOG_ERROR("Failed to attach game system \"{}\" to game instance!",
            Reflection::GetName(gameSystem).GetString());
        return Common::Failure(AttachSystemErrors::FailedAttach);
    }

    GameSystem* attachedGameSystem = m_gameSystemList.emplace_back(std::move(gameSystem)).get();
    const auto [it, result] = m_gameSystemMap.emplace(gameSystemType, attachedGameSystem);
    ASSERT(result, "Failed to map game system!");

    return Common::Success();
}

GameSystem* GameInstance::GetSystem(const Reflection::TypeIdentifier typeIdentifier) const
{
    const auto it = m_gameSystemMap.find(typeIdentifier);
    if(it == m_gameSystemMap.end())
    {
        LOG_WARNING("Game instance does not have game system \"{}\" attached!",
            Reflection::GetName(typeIdentifier).GetString());
        return nullptr;
    }

    GameSystem* gameSystem = it->second;
    ASSERT(gameSystem != nullptr);

    return gameSystem;
}

bool GameInstance::Finalize()
{
    if(m_finalization != FinalizationStates::Pending)
    {
        ASSERT(false, "Failed to finalize game instance due to invalid state!");
        return false;
    }

    for(auto& gameSystemIt : m_gameSystemMap)
    {
        if(!gameSystemIt.second->OnFinalize(this))
        {
            LOG_ERROR("Failed to finalize game system \"{}\"!",
                Reflection::GetName(gameSystemIt.first));
            m_finalization = FinalizationStates::Failed;
            return false;
        }
    }

    m_finalization = FinalizationStates::Done;
    return true;
}

void GameInstance::Tick(const float timeDelta)
{
    ASSERT(m_finalization == FinalizationStates::Done,
        "Attempted to tick game instance that is not finalized!");

    for(auto& gameSystem : m_gameSystemList)
    {
        gameSystem->OnTick(timeDelta);
    }
}
