/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    Graphics Texture
    
    Encapsulates an OpenGL texture object which can be loaded from PNG file.
    
    void ExampleGraphicsTexture(Graphics::RenderContext* renderContext)
    {
        // Load a texture from file.
        Graphics::TextureLoadInfo textureInfo;
        textureLoadInfo.filePath = "image.png";

        Graphics::Texture texture;
        texture.Initialize(renderContext, textureInfo);
        
        // Retrieve the OpenGL handle.
        GLuint handle = texture.GetHandle();

        // Enable a texture unit.
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, handle);
        glUniform1i(shader.GetUniform("texture"), 0);
    }
*/

namespace Graphics
{
    // Forward declarations.
    class RenderContext;

    // Texture info structures.
    struct TextureLoadInfo
    {
        TextureLoadInfo();

        std::string filePath;
        bool mipmaps;
    };

    struct TextureCreateInfo
    {
        TextureCreateInfo();

        int width;
        int height;
        GLenum format;
        bool mipmaps;
        const void* data;
    };

    // Texture class.
    class Texture
    {
    public:
        Texture();
        ~Texture();

        // Disallow copying.
        Texture(const Texture& other) = delete;
        Texture& operator=(const Texture& other) = delete;

        // Move constructor and operator.
        Texture(Texture&& other);
        Texture& operator=(Texture&& other);

        // Loads the texture from a file.
        bool Initialize(RenderContext* renderContext, const TextureLoadInfo& info);

        // Creates a texture instance from memory.
        bool Initialize(RenderContext* renderContext, const TextureCreateInfo& info);

        // Updates the texture data.
        void Update(const void* data);

        // Gets the texture's handle.
        GLuint GetHandle() const;

        // Gets the texture's width.
        int GetWidth() const;

        // Gets the texture's height.
        int GetHeight() const;

        // Checks if the texture instance is valid.
        bool IsValid() const;

    private:
        // Destroys the internal handle.
        void DestroyHandle();

    private:
        // Render context.
        RenderContext* m_renderContext;

        // Texture handle.
        GLuint m_handle;
        GLenum m_format;

        // Texture parameters.
        int m_width;
        int m_height;
    };
    
    // Pointer type.
    using TexturePtr = std::shared_ptr<Texture>;
}
