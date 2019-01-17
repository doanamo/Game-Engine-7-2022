/*
    Copyright (c) 2018-2019 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Game/Components/SpriteComponent.hpp"
#include "Game/Components/TransformComponent.hpp"
#include "Game/ComponentSystem.hpp"
using namespace Game;

SpriteComponent::SpriteComponent() :
    m_transformComponent(nullptr),
    m_rectangle(0.0f, 0.0f, 1.0f, 1.0f),
    m_color(1.0f, 1.0f, 1.0f, 1.0f),
    m_transparent(false),
    m_filtered(true)
{
}

SpriteComponent::~SpriteComponent()
{
}

SpriteComponent::SpriteComponent(SpriteComponent&& other) :
    SpriteComponent()
{
    *this = std::move(other);
}

SpriteComponent& SpriteComponent::operator=(SpriteComponent&& other)
{
    std::swap(m_transformComponent, other.m_transformComponent);
    std::swap(m_textureView, other.m_textureView);
    std::swap(m_rectangle, other.m_rectangle);
    std::swap(m_color, other.m_color);
    std::swap(m_transparent, other.m_transparent);
    std::swap(m_filtered, other.m_filtered);

    return *this;
}

bool SpriteComponent::OnInitialize(ComponentSystem* componentSystem, const EntityHandle& entitySelf)
{
    m_transformComponent = componentSystem->Lookup<TransformComponent>(entitySelf);
    if(m_transformComponent == nullptr) return false;

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
