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
        Graphics::Texture texture(renderContext);
        texture.Load("image.png");
        
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

    // Texture info structure.
    struct TextureInfo
    {
        TextureInfo();

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
        Texture(RenderContext* renderContext);
        ~Texture();

        // Loads the texture from a file.
        bool Load(std::string filePath);

        // Initializes the texture instance.
        bool Create(const TextureInfo& info);

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
}
