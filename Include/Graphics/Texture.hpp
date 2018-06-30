/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    Graphics Texture
    
    Encapsulates an OpenGL texture object which can be loaded from PNG file.
    
    void ExampleGraphicsTexture()
    {
        // Load a texture from file.
        Graphics::Texture texture;
        texture.Load("image.png");
        
        // Retrieve the OpenGL handle.
        GLuint handle = texture.GetHandle();
    }
*/

namespace Graphics
{
    // Texture class.
    class Texture
    {
    public:
        Texture();
        ~Texture();

        // Loads the texture from a file.
        bool Load(std::string filePath);

        // Initializes the texture instance.
        bool Create(int width, int height, GLenum format, const void* data);

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
        // Texture handle.
        GLuint m_handle;

        // Texture parameters.
        GLenum m_format;
        int m_width;
        int m_height;
    };
}
