/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "../Precompiled.hpp"
#include "Game/Systems/SpriteSystem.hpp"
#include "Game/Components/SpriteAnimationComponent.hpp"
#include "Game/Components/SpriteComponent.hpp"
#include "Game/ComponentSystem.hpp"
#include <Graphics/Sprite/SpriteAnimationList.hpp>
using namespace Game;

SpriteSystem::SpriteSystem() = default;
SpriteSystem::~SpriteSystem() = default;

SpriteSystem::CreateResult SpriteSystem::Create(ComponentSystem* componentSystem)
{
    LOG("Create sprite system...");
    LOG_SCOPED_INDENT();

    // Check arguments.
    CHECK_ARGUMENT_OR_RETURN(componentSystem != nullptr, Common::Failure(CreateErrors::InvalidArgument));

    // Create instance.
    auto instance = std::unique_ptr<SpriteSystem>(new SpriteSystem());

    // Save component system reference.
    instance->m_componentSystem = componentSystem;

    // Success!
    return Common::Success(std::move(instance));
}

void SpriteSystem::Tick(float timeDelta)
{
    // Get all sprite animation components.
    for(auto& spriteAnimationComponent : m_componentSystem->GetPool<SpriteAnimationComponent>())
    {
        // Tick sprite animation component.
        spriteAnimationComponent.Tick(timeDelta);
    }
}
