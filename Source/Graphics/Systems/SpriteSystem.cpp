/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Graphics/Precompiled.hpp"
#include "Graphics/Systems/SpriteSystem.hpp"
#include "Graphics/Components/SpriteAnimationComponent.hpp"
#include "Graphics/Components/SpriteComponent.hpp"
#include "Graphics/Sprite/SpriteAnimationList.hpp"
#include <Game/ComponentSystem.hpp>
using namespace Graphics;

SpriteSystem::SpriteSystem() = default;
SpriteSystem::~SpriteSystem() = default;

SpriteSystem::CreateResult SpriteSystem::Create(Game::ComponentSystem* componentSystem)
{
    LOG("Create sprite system...");
    LOG_SCOPED_INDENT();

    // Check arguments.
    CHECK_ARGUMENT_OR_RETURN(componentSystem != nullptr,
        Common::Failure(CreateErrors::InvalidArgument));

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
