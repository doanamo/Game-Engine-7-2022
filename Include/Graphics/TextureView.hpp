/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
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

        TextureView(const TextureView& other);
        TextureView& operator=(const TextureView& other);

        TextureView(TextureView&& other);
        TextureView& operator=(TextureView&& other);

        void SetTexture(ConstTexturePtr texture);
        void SetImageRect(const glm::ivec4 pixelRect);
        void SetTextureRect(const glm::vec4 textureRect);

        ConstTexturePtr GetTexture() const;
        const Texture* GetTexturePtr() const;
        glm::ivec4 GetImageRect() const;
        glm::vec4 GetTextureRect() const;

    private:
        ConstTexturePtr m_texture;
        glm::vec4 m_textureRect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    };
}
