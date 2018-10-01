/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Graphics/TextureView.hpp"
#include "Graphics/Texture.hpp"
using namespace Graphics;

TextureView::TextureView() :
    m_texture(nullptr),
    m_coordinates(0.0f, 0.0f, 1.0f, 1.0f)
{
}

TextureView::~TextureView()
{
}

TextureView::TextureView(ConstTexturePtr texture) :
    TextureView()
{
    this->SetTexture(texture);
}

TextureView::TextureView(ConstTexturePtr texture, glm::ivec4 rectangle) :
    TextureView()
{
    this->SetTexture(texture);
    this->SetRectangle(rectangle);
}

TextureView::TextureView(ConstTexturePtr texture, glm::vec4 coordinates) :
    TextureView()
{
    this->SetTexture(texture);
    this->SetCoordinates(coordinates);
}

TextureView::TextureView(const TextureView& other)
{
    *this = other;
}

TextureView& TextureView::operator=(const TextureView& other)
{
    m_texture = other.m_texture;
    m_coordinates = other.m_coordinates;

    return *this;
}

TextureView::TextureView(TextureView&& other)
{
    *this = std::move(other);
}

TextureView& TextureView::operator=(TextureView&& other)
{
    std::swap(m_texture, other.m_texture);
    std::swap(m_coordinates, other.m_coordinates);

    return *this;
}

void TextureView::SetTexture(ConstTexturePtr texture)
{
    m_texture = texture;
}

void TextureView::SetRectangle(const glm::ivec4 rect)
{
    ASSERT(m_texture != nullptr, "Cannot set texture view rectangle without texture!");

    if(m_texture)
    {
        m_coordinates.x = (float)rect.x / m_texture->GetWidth();
        m_coordinates.y = (float)rect.y / m_texture->GetHeight();
        m_coordinates.z = (float)rect.z / m_texture->GetWidth();
        m_coordinates.w = (float)rect.w / m_texture->GetHeight();
    }
    else
    {
        m_coordinates = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    }
}

void TextureView::SetCoordinates(const glm::vec4 coords)
{
    m_coordinates = coords;
}

TextureView::ConstTexturePtr TextureView::GetTexture() const
{
    return m_texture;
}

glm::ivec4 TextureView::GetRectangle() const
{
    ASSERT(m_texture != nullptr, "Cannot get texture view rectangle without texture!");

    glm::ivec4 rectangle(0, 0, 0, 0);

    if(m_texture)
    {
        rectangle.x = (int)std::round(m_texture->GetWidth() * m_coordinates.x);
        rectangle.y = (int)std::round(m_texture->GetHeight() * m_coordinates.y);
        rectangle.z = (int)std::round(m_texture->GetWidth() * m_coordinates.z);
        rectangle.w = (int)std::round(m_texture->GetHeight() * m_coordinates.w);
    }

    return rectangle;
}

glm::vec4 TextureView::GetCoordinates() const
{
    return m_coordinates;
}
