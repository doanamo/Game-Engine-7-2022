/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Graphics/RenderState.hpp"

/*
    Graphics Sampler
    
    Encapsulates an OpenGL sampler object that defines texture sampling and filtering properties.
    
    void ExampleGraphicsSampler(Graphics::RenderContext* renderContext)
    {
        // Describe sampler info.
        Graphics::SamplerInfo samplerInfo;
        samplerInfo.textureWrapS = GL_REPEAT;
        samplerInfo.textureWrapR = GL_REPEAT;

        // Create a sampler instance.
        Graphics::Sampler sampler(renderContext);
        sampler.Create(samplerInfo);
        
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
    // Forward declarations.
    class RenderContext;

    // Sampler info structure.
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

    // Sampler class.
    class Sampler
    {
    public:
        Sampler(RenderContext* renderContext);
        ~Sampler();

        // Initializes default parameters.
        static void InitializeDefaults();

        // Initializes the sampler object.
        bool Create(const SamplerInfo& info = SamplerInfo());

        // Sets a sampler's parameter.
        template<typename Type>
        void SetParameter(GLenum parameter, const Type& value);

        // Gets the sampler's handle.
        GLuint GetHandle() const;

        // Checks if the instance valid.
        bool IsValid() const;

    private:
        // Destroys the internal handle.
        void DestroyHandle();

    private:
        // Render context.
        RenderContext* m_renderContext;

        // Sampler handle.
        GLuint m_handle;
    };

    // Template implementations.
    template<>
    inline void Sampler::SetParameter<GLint>(GLenum parameter, const GLint& value)
    {
        VERIFY(m_handle != OpenGL::InvalidHandle, "Sampler handle has not been created!");
        glSamplerParameteri(m_handle, parameter, value);
        OpenGL::CheckErrors();
    }

    template<>
    inline void Sampler::SetParameter<GLfloat>(GLenum parameter, const GLfloat& value)
    {
        VERIFY(m_handle != OpenGL::InvalidHandle, "Sampler handle has not been created!");
        glSamplerParameterf(m_handle, parameter, value);
        OpenGL::CheckErrors();
    }

    template<>
    inline void Sampler::SetParameter<glm::vec4>(GLenum parameter, const glm::vec4& value)
    {
        VERIFY(m_handle != OpenGL::InvalidHandle, "Sampler handle has not been created!");
        glSamplerParameterfv(m_handle, parameter, glm::value_ptr(value));
        OpenGL::CheckErrors();
    }
}
