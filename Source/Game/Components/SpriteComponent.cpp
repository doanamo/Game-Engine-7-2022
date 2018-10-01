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
    m_transformComponent(nullptr),
    m_rectangle(0.0f, 0.0f, 1.0f, 1.0f),
    m_color(1.0f, 1.0f, 1.0f, 1.0f),
    m_transparent(false),
    m_filtered(true)
{
}

Sprite::~Sprite()
{
}

void Sprite::SetTextureView(Graphics::TextureView texture)
{
    m_textureView = texture;
}

void Sprite::SetRectangle(const glm::vec4& rectangle)
{
    m_rectangle = rectangle;
}

void Sprite::SetColor(const glm::vec4& color)
{
    m_color = color;
}

void Sprite::SetTransparent(bool toggle)
{
    m_transparent = toggle;
}

void Sprite::SetFiltered(bool toggle)
{
    m_filtered = toggle;
}

const Graphics::TextureView& Sprite::GetTextureView() const
{
    return m_textureView;
}

glm::vec4 Sprite::GetRectangle() const
{
    return m_rectangle;
}

glm::vec4 Sprite::GetColor() const
{
    return m_color;
}

bool Sprite::IsTransparent() const
{
    return m_transparent;
}

bool Sprite::IsFiltered() const
{
    return m_filtered;
}

bool Sprite::OnInitialize(ComponentSystem* componentSystem, const EntityHandle& entitySelf)
{
    m_transformComponent = componentSystem->Lookup<Transform>(entitySelf);
    if(m_transformComponent == nullptr) return false;

    return true;
}

Transform* Sprite::GetTransformComponent() const
{
    ASSERT(m_transformComponent != nullptr, "Returning not yet acquired transform!");

    return m_transformComponent;
}
