/*
    Copyright (c) 2018-2019 Piotr Doan. All rights reserved.
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
        bool CheckErrors();

        // Constant definitions.
        const GLenum InvalidEnum = GL_INVALID_ENUM;
        const GLuint InvalidHandle = 0;
        const GLuint InvalidAttribute = -1;
        const GLuint InvalidUniform = -1;

        // List of capabilities.
        static const GLenum Capabilities[] =
        {
            // GL_ALPHA_TEST,
            // GL_AUTO_NORMAL,
            GL_BLEND,
            // GL_CLIP_PLANEi,
            // GL_COLOR_LOGIC_OP,
            // GL_COLOR_MATERIAL,
            // GL_COLOR_SUM,
            // GL_COLOR_TABLE,
            // GL_CONVOLUTION_1D,
            // GL_CONVOLUTION_2D,
            GL_CULL_FACE,
            GL_DEPTH_TEST,
            // GL_DITHER,
            // GL_FOG,
            // GL_HISTOGRAM,
            // GL_INDEX_LOGIC_OP,
            // GL_LIGHTi,
            // GL_LIGHTING,
            // GL_LINE_SMOOTH,
            // GL_LINE_STIPPLE,
            // GL_MAP1_COLOR_4,
            // GL_MAP1_INDEX,
            // GL_MAP1_NORMAL,
            // GL_MAP1_TEXTURE_COORD_1,
            // GL_MAP1_TEXTURE_COORD_2,
            // GL_MAP1_TEXTURE_COORD_3,
            // GL_MAP1_TEXTURE_COORD_4,
            // GL_MAP1_VERTEX_3,
            // GL_MAP1_VERTEX_4,
            // GL_MAP2_COLOR_4,
            // GL_MAP2_INDEX,
            // GL_MAP2_NORMAL,
            // GL_MAP2_TEXTURE_COORD_1,
            // GL_MAP2_TEXTURE_COORD_2,
            // GL_MAP2_TEXTURE_COORD_3,
            // GL_MAP2_TEXTURE_COORD_4,
            // GL_MAP2_VERTEX_3,
            // GL_MAP2_VERTEX_4,
            // GL_MINMAX,
            GL_MULTISAMPLE,
            // GL_NORMALIZE,
            // GL_POINT_SMOOTH,
            // GL_POINT_SPRITE,
            // GL_POLYGON_OFFSET_FILL,
            // GL_POLYGON_OFFSET_LINE,
            // GL_POLYGON_OFFSET_POINT,
            // GL_POLYGON_SMOOTH,
            // GL_POLYGON_STIPPLE,
            // GL_POST_COLOR_MATRIX_COLOR_TABLE,
            // GL_POST_CONVOLUTION_COLOR_TABLE,
            // GL_RESCALE_NORMAL,
            // GL_SAMPLE_ALPHA_TO_COVERAGE,
            // GL_SAMPLE_ALPHA_TO_ONE,
            // GL_SAMPLE_COVERAGE,
            // GL_SEPARABLE_2D,
            GL_SCISSOR_TEST,
            GL_STENCIL_TEST,
            // GL_TEXTURE_1D,
            // GL_TEXTURE_2D,
            // GL_TEXTURE_3D,
            // GL_TEXTURE_CUBE_MAP,
            // GL_TEXTURE_GEN_Q,
            // GL_TEXTURE_GEN_R,
            // GL_TEXTURE_GEN_S,
            // GL_TEXTURE_GEN_T,
            // GL_VERTEX_PROGRAM_POINT_SIZE,
            // GL_VERTEX_PROGRAM_TWO_SIDE,
        };

        const std::size_t CapabilityCount = Utility::StaticArraySize(Capabilities);

        // List of buffer binding targets.
        static const std::tuple<GLenum, GLenum> BufferBindingTargets[] =
        {
            { GL_ARRAY_BUFFER, GL_ARRAY_BUFFER_BINDING },
            // { GL_ATOMIC_COUNTER_BUFFER, GL_ATOMIC_COUNTER_BUFFER_BINDING },
            // { GL_COPY_READ_BUFFER, GL_COPY_READ_BUFFER_BINDING },
            // { GL_COPY_WRITE_BUFFER, GL_COPY_WRITE_BUFFER_BINDING },
            // { GL_DISPATCH_INDIRECT_BUFFER, GL_DISPATCH_INDIRECT_BUFFER_BINDING },
            // { GL_DRAW_INDIRECT_BUFFER, GL_DRAW_INDIRECT_BUFFER_BINDING },
            { GL_ELEMENT_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER_BINDING },
            // { GL_PIXEL_PACK_BUFFER, GL_PIXEL_PACK_BUFFER_BINDING },
            // { GL_PIXEL_UNPACK_BUFFER, GL_PIXEL_UNPACK_BUFFER_BINDING },
            // { GL_QUERY_BUFFER, GL_QUERY_BUFFER_BINDING },
            // { GL_SHADER_STORAGE_BUFFER, GL_SHADER_STORAGE_BUFFER_BINDING },
            // { GL_TEXTURE_BUFFER, GL_TEXTURE_BUFFER_BINDING },
            // { GL_TRANSFORM_FEEDBACK_BUFFER, GL_TRANSFORM_FEEDBACK_BUFFER_BINDING },
            // { GL_UNIFORM_BUFFER, GL_UNIFORM_BUFFER_BINDING },
        };

        const std::size_t BufferBindingTargetCount = Utility::StaticArraySize(BufferBindingTargets);

        // List of texture binding targets.
        static const std::tuple<GLenum, GLenum> TextureBindingTargets[] =
        {
            // { GL_TEXTURE_1D, GL_TEXTURE_BINDING_1D },
            { GL_TEXTURE_2D, GL_TEXTURE_BINDING_2D },
            // { GL_TEXTURE_3D, GL_TEXTURE_BINDING_3D },
            // { GL_TEXTURE_1D_ARRAY, GL_TEXTURE_BINDING_1D_ARRAY },
            // { GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BINDING_2D_ARRAY },
            // { GL_TEXTURE_RECTANGLE, GL_TEXTURE_BINDING_RECTANGLE },
            // { GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BINDING_CUBE_MAP },
            // { GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_BINDING_CUBE_MAP_ARRAY },
            // { GL_TEXTURE_BUFFER, GL_TEXTURE_BINDING_BUFFER },
            // { GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_BINDING_2D_MULTISAMPLE },
            // { GL_TEXTURE_2D_MULTISAMPLE_ARRAY, GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY },
        };

        const std::size_t TextureBindingTargetCount = Utility::StaticArraySize(TextureBindingTargets);

        // List of pixel store parameters.
        static const GLenum PixelStoreParameters[] =
        {
            // GL_PACK_SWAP_BYTES,
            // GL_PACK_LSB_FIRST,
            // GL_PACK_ROW_LENGTH,
            // GL_PACK_IMAGE_HEIGHT,
            // GL_PACK_SKIP_ROWS,
            // GL_PACK_SKIP_PIXELS,
            // GL_PACK_SKIP_IMAGES,
            GL_PACK_ALIGNMENT,
            // GL_UNPACK_SWAP_BYTES,
            // GL_UNPACK_LSB_FIRST,
            // GL_UNPACK_ROW_LENGTH,
            // GL_UNPACK_IMAGE_HEIGHT,
            // GL_UNPACK_SKIP_ROWS,
            // GL_UNPACK_SKIP_PIXELS,
            // GL_UNPACK_SKIP_IMAGES,
            GL_UNPACK_ALIGNMENT,
        };

        const std::size_t PixelStoreParameterCount = Utility::StaticArraySize(PixelStoreParameters);
    }

    // Render state structure.
    class RenderState
    {
    public:
        RenderState();
        ~RenderState();

        // Copy constructor and operator.
        // We can use defaults as we are only using copyable primitive types.
        RenderState(const RenderState& other) = default;
        RenderState& operator=(const RenderState& other) = default;

        // Move constructor and operator.
        RenderState(RenderState&& other);
        RenderState& operator=(RenderState&& other);

        // Initializes the render state.
        bool Initialize();

        // Apply changes from another state.
        void Apply(RenderState& other);

        // Functions that change OpenGL state.
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

        void ClearDepth(GLdouble depth);
        GLdouble GetClearDepth() const;

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

        // Functions that perform OpenGL operations.
        void Clear(GLbitfield mask);
        void DrawArrays(GLenum mode, GLint first, GLsizei count);
        void DrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);

    private:
        // Initialization state.
        bool m_initialized;

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
        GLdouble m_clearDepth;

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
