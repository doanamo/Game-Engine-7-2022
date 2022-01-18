/*
    Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Game/Systems/SpriteSystem.hpp"
#include "Game/Components/SpriteAnimationComponent.hpp"
#include "Game/Components/SpriteComponent.hpp"
#include "Game/ComponentSystem.hpp"
#include "Game/GameInstance.hpp"
#include <Graphics/Sprite/SpriteAnimationList.hpp>
using namespace Game;

SpriteSystem::SpriteSystem() = default;
SpriteSystem::~SpriteSystem() = default;

bool SpriteSystem::OnAttach(const GameSystemStorage& gameSystems)
{
    // Retrieve needed game systems.
    m_componentSystem = &gameSystems.Locate<ComponentSystem>();

    return true;
}

void SpriteSystem::OnTick(const float timeDelta)
{
    // Tick all sprite animation components.
    for(auto& spriteAnimationComponent : m_componentSystem->GetPool<SpriteAnimationComponent>())
    {
        spriteAnimationComponent.Tick(timeDelta);
    }
}
