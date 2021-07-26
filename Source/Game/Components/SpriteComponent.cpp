/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Game/Precompiled.hpp"
#include "Game/Components/SpriteComponent.hpp"
#include "Game/Components/TransformComponent.hpp"
#include "Game/ComponentSystem.hpp"
using namespace Game;

SpriteComponent::SpriteComponent() = default;
SpriteComponent::~SpriteComponent() = default;

bool SpriteComponent::OnInitialize(ComponentSystem* componentSystem, const EntityHandle& entitySelf)
{
    // Retrieve needed components.
    m_transformComponent = componentSystem->Lookup<
        TransformComponent>(entitySelf).UnwrapOr(nullptr);
    if(m_transformComponent == nullptr)
        return false;

    return true;
}
