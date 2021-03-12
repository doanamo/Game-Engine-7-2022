/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Graphics/Precompiled.hpp"
#include "Graphics/Sampler.hpp"
#include "Graphics/RenderContext.hpp"
using namespace Graphics;

namespace
{
    bool DefaultsInitialized = false;

    GLint DefaultTextureMinFilter = GL_NEAREST_MIPMAP_LINEAR;
    GLint DefaultTextureMagFilter = GL_LINEAR;
    GLint DefaultTextureWrapS = GL_REPEAT;
    GLint DefaultTextureWrapT = GL_REPEAT;
    GLint DefaultTextureWrapR = GL_REPEAT;
    GLfloat DefaultTextureMinLOD = -1000.0f;
    GLfloat DefaultTextureMaxLOD = 1000.0f;
    GLint DefaultTextureCompareMode = GL_NONE;
    GLint DefaultTextureCompareFunc = GL_LEQUAL;

    void InitializeDefaults()
    {
        // Check if already initialized.
        if(DefaultsInitialized)
            return;

        // Create temporary sampler.
        GLuint defaultSampler = OpenGL::InvalidHandle;
        glGenSamplers(1, &defaultSampler);
        OpenGL::CheckErrors();

        ASSERT(defaultSampler != OpenGL::InvalidHandle, "Default sampler handle is invalid!");
        SCOPE_GUARD([&defaultSampler]
        {
            glDeleteSamplers(1, &defaultSampler);
        });

        // Read default parameters.
        glGetSamplerParameteriv(defaultSampler, GL_TEXTURE_MIN_FILTER, &DefaultTextureMinFilter);
        glGetSamplerParameteriv(defaultSampler, GL_TEXTURE_MAG_FILTER, &DefaultTextureMagFilter);
        glGetSamplerParameteriv(defaultSampler, GL_TEXTURE_WRAP_S, &DefaultTextureWrapS);
        glGetSamplerParameteriv(defaultSampler, GL_TEXTURE_WRAP_T, &DefaultTextureWrapT);
        glGetSamplerParameteriv(defaultSampler, GL_TEXTURE_WRAP_R, &DefaultTextureWrapR);
        glGetSamplerParameterfv(defaultSampler, GL_TEXTURE_MIN_LOD, &DefaultTextureMinLOD);
        glGetSamplerParameterfv(defaultSampler, GL_TEXTURE_MAX_LOD, &DefaultTextureMaxLOD);
        glGetSamplerParameteriv(defaultSampler, GL_TEXTURE_COMPARE_MODE, &DefaultTextureCompareMode);
        glGetSamplerParameteriv(defaultSampler, GL_TEXTURE_COMPARE_FUNC, &DefaultTextureCompareFunc);
        OpenGL::CheckErrors();

        // Success!
        DefaultsInitialized = true;
    }
}

Sampler::CreateFromParams::CreateFromParams()
{
    InitializeDefaults();

    renderContext = nullptr;
    textureMinFilter = DefaultTextureMinFilter;
    textureMagFilter = DefaultTextureMagFilter;
    textureWrapS = DefaultTextureWrapS;
    textureWrapT = DefaultTextureWrapT;
    textureWrapR = DefaultTextureWrapR;
    textureMinLOD = DefaultTextureMinLOD;
    textureMaxLOD = DefaultTextureMaxLOD;
    textureCompareMode = DefaultTextureCompareMode;
    textureCompareFunc = DefaultTextureCompareFunc;
}

Sampler::Sampler() = default;

Sampler::~Sampler()
{
    if(m_handle != OpenGL::InvalidHandle)
    {
        glDeleteSamplers(1, &m_handle);
        OpenGL::CheckErrors();
    }
}

Sampler::CreateResult Sampler::Create(const CreateFromParams& params)
{
    LOG("Creating sampler...");
    LOG_SCOPED_INDENT();

    // Validate arguments.
    CHECK_ARGUMENT_OR_RETURN(params.renderContext != nullptr, Common::Failure(CreateErrors::InvalidArgument));

    // Create instance.
    auto instance = std::unique_ptr<Sampler>(new Sampler());

    // Create sampler handle.
    glGenSamplers(1, &instance->m_handle);
    OpenGL::CheckErrors();

    if(instance->m_handle == OpenGL::InvalidHandle)
    {
        LOG_ERROR("Sampler could not be created!");
        return Common::Failure(CreateErrors::FailedResourceCreation);
    }

    // Set sampling parameters.
    if(DefaultTextureMinFilter != params.textureMinFilter)
    {
        glSamplerParameteri(instance->m_handle, GL_TEXTURE_MIN_FILTER, params.textureMinFilter);
    }

    if(DefaultTextureMagFilter != params.textureMagFilter)
    {
        glSamplerParameteri(instance->m_handle, GL_TEXTURE_MAG_FILTER, params.textureMagFilter);
    }

    if(DefaultTextureWrapS != params.textureWrapS)
    {
        glSamplerParameteri(instance->m_handle, GL_TEXTURE_WRAP_S, params.textureWrapS);
    }

    if(DefaultTextureWrapT != params.textureWrapT)
    {
        glSamplerParameteri(instance->m_handle, GL_TEXTURE_WRAP_T, params.textureWrapT);
    }

    if(DefaultTextureWrapR != params.textureWrapR)
    {
        glSamplerParameteri(instance->m_handle, GL_TEXTURE_WRAP_R, params.textureWrapR);
    }

    if(DefaultTextureMinLOD != params.textureMinLOD)
    {
        glSamplerParameterf(instance->m_handle, GL_TEXTURE_MIN_LOD, params.textureMinLOD);
    }

    if(DefaultTextureMaxLOD != params.textureMaxLOD)
    {
        glSamplerParameterf(instance->m_handle, GL_TEXTURE_MAX_LOD, params.textureMaxLOD);
    }

    if(DefaultTextureCompareMode != params.textureCompareMode)
    {
        glSamplerParameteri(instance->m_handle, GL_TEXTURE_COMPARE_MODE, params.textureCompareMode);
    }

    if(DefaultTextureCompareFunc != params.textureCompareFunc)
    {
        glSamplerParameteri(instance->m_handle, GL_TEXTURE_COMPARE_FUNC, params.textureCompareFunc);
    }

    // Save render context reference.
    instance->m_renderContext = params.renderContext;

    // Success!
    return Common::Success(std::move(instance));
}

GLuint Sampler::GetHandle() const
{
    return m_handle;
}
