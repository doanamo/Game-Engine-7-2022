/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

/*
    Texture View

    Utility object used to represent a rectangular area on image or texture.
*/

namespace Graphics
{
    class Texture;

    class TextureView
    {
    public:
        using ConstTexturePtr = std::shared_ptr<const Texture>;

    public:
        TextureView();
        ~TextureView();

        TextureView(ConstTexturePtr texture);
        TextureView(ConstTexturePtr texture, glm::ivec4 imageRect);
        TextureView(ConstTexturePtr texture, glm::vec4 textureRect);

        TextureView(const TextureView& other) = default;
        TextureView& operator=(const TextureView& other) = default;;

        TextureView(TextureView&& other);
        TextureView& operator=(TextureView&& other);

        void SetTexture(ConstTexturePtr texture)
        {
            m_texture = texture;
        }

        void SetTextureRect(const glm::vec4 normalRect)
        {
            m_textureRect = normalRect;
        }

        void SetImageRect(const glm::ivec4 pixelRect);

        ConstTexturePtr GetTexture() const
        {
            return m_texture;
        }

        const Texture* GetTexturePtr() const
        {
            return m_texture.get();
        }

        glm::vec4 GetTextureRect() const
        {
            return m_textureRect;
        }

        glm::ivec4 GetImageRect() const;

    private:
        ConstTexturePtr m_texture;
        glm::vec4 m_textureRect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    };
}
