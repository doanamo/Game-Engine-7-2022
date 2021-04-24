/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Game/Precompiled.hpp"
#include "Game/Systems/SpriteSystem.hpp"
#include "Game/Components/SpriteAnimationComponent.hpp"
#include "Game/Components/SpriteComponent.hpp"
#include "Game/ComponentSystem.hpp"
#include "Game/GameInstance.hpp"
#include <Graphics/Sprite/SpriteAnimationList.hpp>
using namespace Game;

SpriteSystem::SpriteSystem() = default;
SpriteSystem::~SpriteSystem() = default;

bool SpriteSystem::OnAttach(GameInstance* gameInstance)
{
    ASSERT(m_componentSystem == nullptr);

    m_componentSystem = gameInstance->GetSystem<ComponentSystem>();
    if(m_componentSystem == nullptr)
    {
        LOG_ERROR("Could not retrieve component system!");
        return false;
    }

    return true;
}

void SpriteSystem::OnTick(const float timeDelta)
{
    // Get all sprite animation components.
    for(auto& spriteAnimationComponent : m_componentSystem->GetPool<SpriteAnimationComponent>())
    {
        // Tick sprite animation component.
        spriteAnimationComponent.Tick(timeDelta);
    }
}
