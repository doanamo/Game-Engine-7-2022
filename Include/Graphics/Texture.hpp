/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Core/EngineSystem.hpp>
#include "Graphics/RenderState.hpp"

namespace System
{
    class FileHandle;
}

/*
    Texture
    
    Encapsulates an OpenGL texture object which can be loaded from PNG file.
*/

namespace Graphics
{
    class RenderContext;

    class Texture final : private Common::NonCopyable
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
            const Core::EngineSystemStorage* engineSystems = nullptr;
            bool mipmaps = true;
        };

        enum class CreateErrors
        {
            InvalidArgument,
            UnsupportedImageFormat,
            FailedTextureCreation,
            FailedImageLoad,
        };

        using CreateResult = Common::Result<std::unique_ptr<Texture>, CreateErrors>;
        static CreateResult Create(const CreateFromParams& params);
        static CreateResult Create(System::FileHandle& file, const LoadFromFile& params);

    public:
        ~Texture();
        void Update(const void* data);

        GLuint GetHandle() const
        {
            return m_handle;
        }

        int GetWidth() const
        {
            return m_width;
        }

        int GetHeight() const
        {
            return m_height;
        }

    private:
        Texture();

    private:
        RenderContext* m_renderContext = nullptr;
        GLuint m_handle = OpenGL::InvalidHandle;
        GLenum m_format = OpenGL::InvalidEnum;
        int m_width = 0;
        int m_height = 0;
    };
    
    using TexturePtr = std::shared_ptr<Texture>;
    using ConstTexturePtr = std::shared_ptr<const Texture>;
}
