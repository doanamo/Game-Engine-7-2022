/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Game/Components/SpriteComponent.hpp"
#include "Game/Components/TransformComponent.hpp"
#include "Game/ComponentSystem.hpp"

using namespace Game;
using namespace Components;

Sprite::Sprite() :
    m_transform(nullptr)
{
}

Sprite::~Sprite()
{
}

bool Sprite::OnInitialize(ComponentSystem* componentSystem, const EntityHandle& entitySelf)
{
    m_transform = componentSystem->Lookup<Transform>(entitySelf);
    if(m_transform == nullptr) return false;

    return true;
}

Transform* Sprite::GetTransform() const
{
    ASSERT(m_transform != nullptr, "Returning not yet acquired transform!");

    return m_transform;
}
