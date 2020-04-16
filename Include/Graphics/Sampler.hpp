/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Graphics/RenderState.hpp"

/*
    Sampler
    
    Encapsulates an OpenGL sampler object that defines texture sampling and filtering properties.
    
    void ExampleGraphicsSampler(Graphics::RenderContext* renderContext)
    {
        // Describe sampler info.
        Graphics::SamplerInfo samplerInfo;
        samplerInfo.textureWrapS = GL_REPEAT;
        samplerInfo.textureWrapR = GL_REPEAT;

        // Create sampler instance.
        Graphics::Sampler sampler();
        sampler.Initialize(renderContext, samplerInfo);
        
        // Modify parameters after creation.
        sampler.SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        sampler.SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Retrieve and bind OpenGL sampler.
        GLuint handle = sampler.GetHandle();
        glBindSampler(0, handle);
    }
*/

namespace Graphics
{
    class RenderContext;

    struct SamplerInfo
    {
        SamplerInfo();

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

    class Sampler final : private NonCopyable, public Resettable<Sampler>
    {
    public:
        enum class InitializeErrors
        {
            InvalidArgument,
            FailedResourceCreation,
        };

        using InitializeResult = Result<void, InitializeErrors>;

    public:
        Sampler();
        ~Sampler();

        InitializeResult Initialize(RenderContext* renderContext, const SamplerInfo& info = SamplerInfo());

        template<typename Type>
        void SetParameter(GLenum parameter, const Type& value);

        GLuint GetHandle() const;
        bool IsInitialized() const;

    private:
        RenderContext* m_renderContext = nullptr;
        GLuint m_handle = OpenGL::InvalidHandle;
        bool m_initialized = false;
    };

    template<>
    inline void Sampler::SetParameter<GLint>(GLenum parameter, const GLint& value)
    {
        ASSERT(m_initialized, "Sampler has not been initialized!");
        glSamplerParameteri(m_handle, parameter, value);
        OpenGL::CheckErrors();
    }

    template<>
    inline void Sampler::SetParameter<GLfloat>(GLenum parameter, const GLfloat& value)
    {
        ASSERT(m_initialized, "Sampler has not been initialized!");
        glSamplerParameterf(m_handle, parameter, value);
        OpenGL::CheckErrors();
    }

    template<>
    inline void Sampler::SetParameter<glm::vec4>(GLenum parameter, const glm::vec4& value)
    {
        ASSERT(m_initialized, "Sampler has not been initialized!");
        glSamplerParameterfv(m_handle, parameter, glm::value_ptr(value));
        OpenGL::CheckErrors();
    }
}
