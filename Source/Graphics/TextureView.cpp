/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Graphics/TextureView.hpp"
#include "Graphics/Texture.hpp"
using namespace Graphics;

TextureView::TextureView() :
    m_texture(nullptr),
    m_normalRect(0.0f, 0.0f, 1.0f, 1.0f)
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

TextureView::TextureView(ConstTexturePtr texture, glm::ivec4 pixelRect) :
    TextureView()
{
    this->SetTexture(texture);
    this->SetPixelRect(pixelRect);
}

TextureView::TextureView(ConstTexturePtr texture, glm::vec4 normalRect) :
    TextureView()
{
    this->SetTexture(texture);
    this->SetNormalRect(normalRect);
}

TextureView::TextureView(const TextureView& other)
{
    *this = other;
}

TextureView& TextureView::operator=(const TextureView& other)
{
    m_texture = other.m_texture;
    m_normalRect = other.m_normalRect;

    return *this;
}

TextureView::TextureView(TextureView&& other)
{
    *this = std::move(other);
}

TextureView& TextureView::operator=(TextureView&& other)
{
    std::swap(m_texture, other.m_texture);
    std::swap(m_normalRect, other.m_normalRect);

    return *this;
}

void TextureView::SetTexture(ConstTexturePtr texture)
{
    m_texture = texture;
}

void TextureView::SetPixelRect(const glm::ivec4 pixelRect)
{
    ASSERT(m_texture != nullptr, "Cannot set texture view pixel rectangle without texture!");

    if(m_texture)
    {
        m_normalRect.x = (float)pixelRect.x / m_texture->GetWidth();
        m_normalRect.y = (float)pixelRect.y / m_texture->GetHeight();
        m_normalRect.z = (float)pixelRect.z / m_texture->GetWidth();
        m_normalRect.w = (float)pixelRect.w / m_texture->GetHeight();
    }
    else
    {
        m_normalRect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    }
}

void TextureView::SetNormalRect(const glm::vec4 normalRect)
{
    m_normalRect = normalRect;
}

TextureView::ConstTexturePtr TextureView::GetTexture() const
{
    return m_texture;
}

const Texture* TextureView::GetTexturePtr() const
{
    return m_texture.get();
}

glm::ivec4 TextureView::GetPixelRect() const
{
    ASSERT(m_texture != nullptr, "Cannot get texture view pixel rectangle without texture!");

    glm::ivec4 rectangle(0, 0, 0, 0);

    if(m_texture)
    {
        rectangle.x = (int)std::round(m_texture->GetWidth() * m_normalRect.x);
        rectangle.y = (int)std::round(m_texture->GetHeight() * m_normalRect.y);
        rectangle.z = (int)std::round(m_texture->GetWidth() * m_normalRect.z);
        rectangle.w = (int)std::round(m_texture->GetHeight() * m_normalRect.w);
    }

    return rectangle;
}

glm::vec4 TextureView::GetNormalRect() const
{
    return m_normalRect;
}
