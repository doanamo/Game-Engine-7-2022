/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
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
    m_transformComponent = componentSystem->Lookup<TransformComponent>(entitySelf);
    if(m_transformComponent == nullptr)
        return false;

    return true;
}

void SpriteComponent::SetTextureView(Graphics::TextureView texture)
{
    m_textureView = texture;
}

void SpriteComponent::SetRectangle(const glm::vec4& rectangle)
{
    m_rectangle = rectangle;
}

void SpriteComponent::SetColor(const glm::vec4& color)
{
    m_color = color;
}

void SpriteComponent::SetTransparent(bool toggle)
{
    m_transparent = toggle;
}

void SpriteComponent::SetFiltered(bool toggle)
{
    m_filtered = toggle;
}

const Graphics::TextureView& SpriteComponent::GetTextureView() const
{
    return m_textureView;
}

glm::vec4 SpriteComponent::GetRectangle() const
{
    return m_rectangle;
}

glm::vec4 SpriteComponent::GetColor() const
{
    return m_color;
}

bool SpriteComponent::IsTransparent() const
{
    return m_transparent;
}

bool SpriteComponent::IsFiltered() const
{
    return m_filtered;
}

TransformComponent* SpriteComponent::GetTransformComponent() const
{
    ASSERT(m_transformComponent != nullptr, "Returning not yet acquired transform!");
    return m_transformComponent;
}
