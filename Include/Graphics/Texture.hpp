/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

// Forward declarations.
namespace Engine
{
    class Root;
}

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

    // Texture class.
    class Texture
    {
    public:
        // Create texture from params.
        struct CreateFromParams
        {
            CreateFromParams();

            Engine::Root* engine;
            int width;
            int height;
            GLenum format;
            bool mipmaps;
            const void* data;
        };

        // Load texture from a file.
        struct LoadFromFile
        {
            LoadFromFile();

            Engine::Root* engine;
            std::string filePath;
            bool mipmaps;
        };

    public:
        Texture();
        ~Texture();

        // Disallow copying.
        Texture(const Texture& other) = delete;
        Texture& operator=(const Texture& other) = delete;

        // Move constructor and operator.
        Texture(Texture&& other);
        Texture& operator=(Texture&& other);

        // Creates a texture instance from parameters.
        bool Initialize(const CreateFromParams& params);

        // Loads the texture from a file.
        bool Initialize(const LoadFromFile& params);

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
    
    // Type declarations.
    using TexturePtr = std::shared_ptr<Texture>;
    using ConstTexturePtr = std::shared_ptr<const Texture>;
}
