/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include <tuple>

/*
    Graphics Render state
*/

namespace Graphics
{
    // OpenGL API enumerations and values.
    namespace OpenGL
    {
        // Error checking method and macro.
        void CheckErrors();

        // Constant definitions.
        const GLenum InvalidEnum = GL_INVALID_ENUM;
        const GLuint InvalidHandle = 0;
        const GLuint InvalidAttribute = -1;
        const GLuint InvalidUniform = -1;

        // List of buffer binding targets.
        static const std::tuple<GLenum, GLenum> BufferBindingTargets[] =
        {
            { GL_ARRAY_BUFFER, GL_ARRAY_BUFFER_BINDING },
            { GL_ATOMIC_COUNTER_BUFFER, GL_ATOMIC_COUNTER_BUFFER_BINDING },
            { GL_COPY_READ_BUFFER, GL_COPY_READ_BUFFER_BINDING },
            { GL_COPY_WRITE_BUFFER, GL_COPY_WRITE_BUFFER_BINDING },
            { GL_DISPATCH_INDIRECT_BUFFER, GL_DISPATCH_INDIRECT_BUFFER_BINDING },
            { GL_DRAW_INDIRECT_BUFFER, GL_DRAW_INDIRECT_BUFFER_BINDING },
            { GL_ELEMENT_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER_BINDING },
            { GL_PIXEL_PACK_BUFFER, GL_PIXEL_PACK_BUFFER_BINDING },
            { GL_PIXEL_UNPACK_BUFFER, GL_PIXEL_UNPACK_BUFFER_BINDING },
            { GL_QUERY_BUFFER, GL_QUERY_BUFFER_BINDING },
            { GL_SHADER_STORAGE_BUFFER, GL_SHADER_STORAGE_BUFFER_BINDING },
            { GL_TEXTURE_BUFFER, GL_TEXTURE_BUFFER_BINDING },
            { GL_TRANSFORM_FEEDBACK_BUFFER, GL_TRANSFORM_FEEDBACK_BUFFER_BINDING },
            { GL_UNIFORM_BUFFER, GL_UNIFORM_BUFFER_BINDING },
        };

        const int BufferBindingTargetCount = Utility::StaticArraySize(BufferBindingTargets);

        // List of texture binding targets.
        static const std::tuple<GLenum, GLenum> TextureBindingTargets[] =
        {
            { GL_TEXTURE_1D, GL_TEXTURE_BINDING_1D },
            { GL_TEXTURE_2D, GL_TEXTURE_BINDING_2D },
            { GL_TEXTURE_3D, GL_TEXTURE_BINDING_3D },
            { GL_TEXTURE_1D_ARRAY, GL_TEXTURE_BINDING_1D_ARRAY },
            { GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BINDING_2D_ARRAY },
            { GL_TEXTURE_RECTANGLE, GL_TEXTURE_BINDING_RECTANGLE },
            { GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BINDING_CUBE_MAP },
            { GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_BINDING_CUBE_MAP_ARRAY },
            { GL_TEXTURE_BUFFER, GL_TEXTURE_BINDING_BUFFER },
            { GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_BINDING_2D_MULTISAMPLE },
            { GL_TEXTURE_2D_MULTISAMPLE_ARRAY, GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY },
        };

        const int TextureBindingTargetCount = Utility::StaticArraySize(TextureBindingTargets);

        // List of pixel store parameters.
        static const GLenum PixelStoreParameters[] =
        {
            GL_PACK_SWAP_BYTES,
            GL_PACK_LSB_FIRST,
            GL_PACK_ROW_LENGTH,
            GL_PACK_IMAGE_HEIGHT,
            GL_PACK_SKIP_ROWS,
            GL_PACK_SKIP_PIXELS,
            GL_PACK_SKIP_IMAGES,
            GL_PACK_ALIGNMENT,
            GL_UNPACK_SWAP_BYTES,
            GL_UNPACK_LSB_FIRST,
            GL_UNPACK_ROW_LENGTH,
            GL_UNPACK_IMAGE_HEIGHT,
            GL_UNPACK_SKIP_ROWS,
            GL_UNPACK_SKIP_PIXELS,
            GL_UNPACK_SKIP_IMAGES,
            GL_UNPACK_ALIGNMENT,
        };

        const int PixelStoreParameterCount = Utility::StaticArraySize(PixelStoreParameters);
    }

    // Render state structure.
    class RenderState
    {
    public:
        RenderState();

        void BindVertexArray(GLuint array);
        GLuint GetVertexArrayBinding() const;

        void BindBuffer(GLenum target, GLuint buffer);
        GLuint GetBufferBinding(GLenum target) const;

        void ActiveTexture(GLenum texture);
        GLenum GetActiveTexture() const;

        void BindTexture(GLenum target, GLuint texture);
        GLuint GetTextureBinding(GLenum target) const;

        void BindSampler(GLuint unit, GLuint sampler);
        GLuint GetSamplerBinding(GLuint unit) const;

        void PixelStore(GLenum pname, GLint param);
        GLint GetPixelStore(GLenum pname) const;

    private:
        // glBindVertexArray
        GLuint m_vertexArrayBinding;

        // glBindBuffer
        GLuint m_bufferBindings[OpenGL::BufferBindingTargetCount];

        // glActiveTexture
        GLenum m_activeTexture;
        
        // glBindTexture
        GLuint m_textureBindings[OpenGL::TextureBindingTargetCount];

        // glBindSampler
        std::vector<GLuint> m_samplerBindings;

        // glPixelStore
        GLint m_pixelStore[OpenGL::PixelStoreParameterCount];
    };
}
