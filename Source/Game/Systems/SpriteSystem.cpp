/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Game/Systems/SpriteSystem.hpp"
#include "Game/Components/SpriteAnimationComponent.hpp"
#include "Game/Components/SpriteComponent.hpp"
#include "Game/ComponentSystem.hpp"
#include <Graphics/Sprite/SpriteAnimationList.hpp>
using namespace Game;

SpriteSystem::SpriteSystem() = default;
SpriteSystem::~SpriteSystem() = default;

SpriteSystem::InitializeResult SpriteSystem::Initialize(ComponentSystem* componentSystem)
{
    LOG("Initializing sprite system...");
    LOG_SCOPED_INDENT();

    // Setup initialization guard.
    VERIFY(!m_initialized, "Instance has already been initialized!");
    SCOPE_GUARD_IF(!m_initialized, this->Reset());

    // Validate component system reference.
    CHECK_ARGUMENT_OR_RETURN(componentSystem != nullptr, Failure(InitializeErrors::InvalidArgument));

    m_componentSystem = componentSystem;

    // Success!
    m_initialized = true;
    return Success();
}

void SpriteSystem::Update(float timeDelta)
{
    ASSERT(m_initialized, "Sprite system has not been initialized!");

    // Get all sprite animation components.
    for(auto& spriteAnimationComponent : m_componentSystem->GetPool<SpriteAnimationComponent>())
    {
        // Update sprite animation component.
        spriteAnimationComponent.Update(timeDelta);
    }
}
