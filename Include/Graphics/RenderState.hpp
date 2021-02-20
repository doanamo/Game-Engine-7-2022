/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <tuple>

/*
    Render state
*/

namespace Graphics
{
    namespace OpenGL
    {
        bool CheckErrors();

        const GLenum InvalidEnum = GL_INVALID_ENUM;
        const GLuint InvalidHandle = 0;
        const GLuint InvalidAttribute = -1;
        const GLuint InvalidUniform = -1;

        static const GLenum Capabilities[] =
        {
            GL_BLEND,
            GL_CULL_FACE,
            GL_DEPTH_TEST,
            GL_SCISSOR_TEST,
            GL_STENCIL_TEST,
        };

        const std::size_t CapabilityCount = Common::StaticArraySize(Capabilities);

        static const std::tuple<GLenum, GLenum> BufferBindingTargets[] =
        {
            { GL_ARRAY_BUFFER, GL_ARRAY_BUFFER_BINDING },
            { GL_ELEMENT_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER_BINDING },
        };

        const std::size_t BufferBindingTargetCount = Common::StaticArraySize(BufferBindingTargets);

        static const std::tuple<GLenum, GLenum> TextureBindingTargets[] =
        {
            { GL_TEXTURE_2D, GL_TEXTURE_BINDING_2D },
        };

        const std::size_t TextureBindingTargetCount = Common::StaticArraySize(TextureBindingTargets);

        static const GLenum PixelStoreParameters[] =
        {
            GL_PACK_ALIGNMENT,
            GL_UNPACK_ALIGNMENT,
        };

        const std::size_t PixelStoreParameterCount = Common::StaticArraySize(PixelStoreParameters);
    }

    class RenderState final : public Common::Resettable<RenderState>
    {
    public:
        RenderState();
        ~RenderState();

        void Save();
        void Apply(RenderState& other);

        void Enable(GLenum cap);
        void Disable(GLenum cap);
        GLboolean IsEnabled(GLenum cap) const;

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

        void UseProgram(GLuint program);
        GLuint GetCurrentProgram() const;

        void Viewport(GLint x, GLint y, GLsizei width, GLsizei height);
        std::tuple<GLint, GLint, GLsizei, GLsizei> GetViewport() const;

        void ClearDepth(GLfloat depth);
        GLfloat GetClearDepth() const;

        void ClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
        std::tuple<GLfloat, GLfloat, GLfloat, GLfloat> GetClearColor() const;

        void DepthMask(GLboolean flag);
        GLboolean GetDepthMask() const;

        void BlendFunc(GLenum sfactor, GLenum dfactor);
        void BlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
        std::tuple<GLenum, GLenum, GLenum, GLenum> GetBlendFuncSeparate() const;

        void BlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha);
        std::tuple<GLenum, GLenum> GetBlendEquationSeperate() const;

        void Scissor(GLint x, GLint y, GLsizei width, GLsizei height);
        std::tuple<GLint, GLint, GLsizei, GLsizei> GetScissorBox() const;

        void Clear(GLbitfield mask);
        void DrawArrays(GLenum mode, GLint first, GLsizei count);
        void DrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);

    private:
        // glEnable
        GLboolean m_capabilities[OpenGL::CapabilityCount];

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

        // glUseProgram
        GLuint m_currentProgram;

        // glViewport
        std::tuple<GLint, GLint, GLsizei, GLsizei> m_viewport;

        // glClearDepth
        GLfloat m_clearDepth;

        // glClearColor
        std::tuple<GLfloat, GLfloat, GLfloat, GLfloat> m_clearColor;

        // glDepthMask
        GLboolean m_depthMask;

        // glBlendFuncSeparate
        std::tuple<GLenum, GLenum, GLenum, GLenum> m_blendFuncSeparate;

        // glBlendEquationSeparate
        std::tuple<GLenum, GLenum> m_blendEquationSeparate;

        // glScissor
        std::tuple<GLint, GLint, GLsizei, GLsizei> m_scissorBox;
    };
}
