/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Components/SpriteComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Game/ComponentSystem.hpp"
using namespace Components;

Sprite::Sprite() :
    m_transform(nullptr)
{
}

Sprite::~Sprite()
{
}

bool Sprite::OnInitialize(Game::ComponentSystem* componentSystem, const Game::EntityHandle& entitySelf)
{
    m_transform = componentSystem->Lookup<Components::Transform>(entitySelf);
    if(m_transform == nullptr) return false;

    return true;
}

Transform* Sprite::GetTransform() const
{
    ASSERT(m_transform != nullptr, "Returning not yet acquired transform!");

    return m_transform;
}
