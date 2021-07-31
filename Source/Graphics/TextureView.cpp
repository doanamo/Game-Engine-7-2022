/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Graphics/Precompiled.hpp"
#include "Graphics/TextureView.hpp"
#include "Graphics/Texture.hpp"
using namespace Graphics;

TextureView::TextureView() = default;
TextureView::~TextureView() = default;

TextureView::TextureView(ConstTexturePtr texture)
    : TextureView()
{
    SetTexture(texture);
}

TextureView::TextureView(ConstTexturePtr texture, glm::ivec4 imageRect)
    : TextureView()
{
    SetTexture(texture);
    SetImageRect(imageRect);
}

TextureView::TextureView(ConstTexturePtr texture, glm::vec4 textureRect)
    : TextureView()
{
    SetTexture(texture);
    SetTextureRect(textureRect);
}

TextureView::TextureView(TextureView&& other)
    : TextureView()
{
    *this = std::move(other);
}

TextureView& TextureView::operator=(TextureView&& other)
{
    std::swap(m_texture, other.m_texture);
    std::swap(m_textureRect, other.m_textureRect);
    return *this;
}

void TextureView::SetImageRect(const glm::ivec4 imageRect)
{
    ASSERT(m_texture != nullptr, "Cannot set image rectangle without texture!");

    if(m_texture)
    {
        // Transform rectangle from image space to texture space.
        // This requires normalizing coordinates and flipping the y-axis.
        m_textureRect.x = (float)imageRect.x / m_texture->GetWidth();
        m_textureRect.y = (float)imageRect.w / m_texture->GetHeight();
        m_textureRect.z = (float)imageRect.z / m_texture->GetWidth();
        m_textureRect.w = (float)imageRect.y / m_texture->GetHeight();
        m_textureRect.y = 1.0f - m_textureRect.y;
        m_textureRect.w = 1.0f - m_textureRect.w;
    }
    else
    {
        m_textureRect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    }
}

glm::ivec4 TextureView::GetImageRect() const
{
    ASSERT(m_texture != nullptr, "Cannot get image rectangle without texture!");

    glm::ivec4 rectangle(0, 0, 0, 0);

    if(m_texture)
    {
        // Transform rectangle from texture space to image space.
        rectangle.x = (int)std::round(m_textureRect.x * m_texture->GetWidth());
        rectangle.y = (int)std::round(m_textureRect.w * m_texture->GetHeight());
        rectangle.z = (int)std::round(m_textureRect.z * m_texture->GetWidth());
        rectangle.w = (int)std::round(m_textureRect.y * m_texture->GetHeight());
        rectangle.y = m_texture->GetHeight() - rectangle.y;
        rectangle.w = m_texture->GetHeight() - rectangle.w;
    }

    return rectangle;
}
