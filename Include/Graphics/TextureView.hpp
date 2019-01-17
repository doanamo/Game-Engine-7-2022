/*
    Copyright (c) 2018-2019 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    Texture View

    Utility object used to represent a rectangular area on image or texture.
*/

namespace Graphics
{
    // Forward declaration.
    class Texture;

    // Texture view class.
    class TextureView
    {
    public:
        // Type declarations.
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

        // Sets the texture reference.
        void SetTexture(ConstTexturePtr texture);

        // Sets the texture rectangle in image space.
        void SetImageRect(const glm::ivec4 pixelRect);

        // Sets the texture rectangle in texture space.
        void SetTextureRect(const glm::vec4 textureRect);

        // Gets the texture reference.
        ConstTexturePtr GetTexture() const;

        // Gets the texture pointer.
        const Texture* GetTexturePtr() const;

        // Gets the texture rectangle coordinates in image space.
        glm::ivec4 GetImageRect() const;

        // Gets the texture rectangle coordinates in texture space.
        glm::vec4 GetTextureRect() const;

    private:
        // Texture reference.
        ConstTexturePtr m_texture;

        // Texture rectangle coordinates in texture space.
        glm::vec4 m_textureRect;
    };
}
