/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Graphics/RenderState.hpp"

/*
    Sampler
    
    Encapsulates OpenGL sampler object that defines texture sampling and filtering properties.
*/

namespace Graphics
{
    class RenderContext;

    class Sampler final : private NonCopyable
    {
    public:
        struct CreateFromParams
        {
            CreateFromParams();

            glm::vec4 textureBorderColor;
            GLint textureMinFilter;
            GLint textureMagFilter;
            GLint textureWrapS;
            GLint textureWrapT;
            GLint textureWrapR;
            GLfloat textureMinLOD;
            GLfloat textureMaxLOD;
            GLfloat textureLODBias;
            GLint textureCompareMode;
            GLint textureCompareFunc;
            GLfloat textureMaxAniso;
        };

        enum class CreateErrors
        {
            InvalidArgument,
            FailedResourceCreation,
        };

        using CreateResult = Result<std::unique_ptr<Sampler>, CreateErrors>;
        static CreateResult Create(RenderContext* renderContext, const CreateFromParams& params);

    public:
        ~Sampler();

        template<typename Type>
        void SetParameter(GLenum parameter, const Type& value);

        GLuint GetHandle() const;

    private:
        Sampler();

    private:
        RenderContext* m_renderContext = nullptr;
        GLuint m_handle = OpenGL::InvalidHandle;
    };

    template<>
    inline void Sampler::SetParameter<GLint>(GLenum parameter, const GLint& value)
    {
        glSamplerParameteri(m_handle, parameter, value);
        OpenGL::CheckErrors();
    }

    template<>
    inline void Sampler::SetParameter<GLfloat>(GLenum parameter, const GLfloat& value)
    {
        glSamplerParameterf(m_handle, parameter, value);
        OpenGL::CheckErrors();
    }

    template<>
    inline void Sampler::SetParameter<glm::vec4>(GLenum parameter, const glm::vec4& value)
    {
        glSamplerParameterfv(m_handle, parameter, glm::value_ptr(value));
        OpenGL::CheckErrors();
    }
}
