/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Graphics/RenderState.hpp"

namespace System
{
    class FileSystem;
}

/*
    Texture
    
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
    class RenderContext;

    class Texture : private NonCopyable
    {
    public:
        struct CreateFromParams
        {
            RenderContext* renderContext = nullptr;
            GLenum format = OpenGL::InvalidEnum;
            int width = 0;
            int height = 0;
            bool mipmaps = true;
            const void* data = nullptr;
        };

        struct LoadFromFile
        {
            System::FileSystem* fileSystem = nullptr;
            RenderContext* renderContext = nullptr;
            std::string filePath;
            bool mipmaps = true;
        };

    public:
        Texture() = default;
        ~Texture();

        Texture(Texture&& other);
        Texture& operator=(Texture&& other);

        bool Initialize(const CreateFromParams& params);
        bool Initialize(const LoadFromFile& params);
        void Update(const void* data);

        GLuint GetHandle() const;
        int GetWidth() const;
        int GetHeight() const;
        bool IsValid() const;

    private:
        void DestroyHandle();

    private:
        RenderContext* m_renderContext;

        GLuint m_handle = OpenGL::InvalidHandle;
        GLenum m_format = OpenGL::InvalidEnum;
        int m_width = 0;
        int m_height = 0;
    };
    
    using TexturePtr = std::shared_ptr<Texture>;
    using ConstTexturePtr = std::shared_ptr<const Texture>;
}
