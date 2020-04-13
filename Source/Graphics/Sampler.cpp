/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Graphics/Sampler.hpp"
#include "Graphics/RenderContext.hpp"
using namespace Graphics;

namespace
{
    bool DefaultsInitialized = false;

    glm::vec4 DefaultTextureBorderColor = { 0.0f, 0.0f, 0.0f, 0.0f };
    GLint DefaultTextureMinFilter = GL_NEAREST_MIPMAP_LINEAR;
    GLint DefaultTextureMagFilter = GL_LINEAR;
    GLint DefaultTextureWrapS = GL_REPEAT;
    GLint DefaultTextureWrapT = GL_REPEAT;
    GLint DefaultTextureWrapR = GL_REPEAT;
    GLfloat DefaultTextureMinLOD = -1000.0f;
    GLfloat DefaultTextureMaxLOD = 1000.0f;
    GLfloat DefaultTextureLODBias = 0.0f;
    GLint DefaultTextureCompareMode = GL_NONE;
    GLint DefaultTextureCompareFunc = GL_LEQUAL;
    GLfloat DefaultTextureMaxAniso = 1.0f;

    void InitializeDefaults()
    {
        // Check if already initialized.
        if(DefaultsInitialized)
            return;

        // Create a temporary sampler.
        GLuint defaultSampler = OpenGL::InvalidHandle;
        glGenSamplers(1, &defaultSampler);
        OpenGL::CheckErrors();

        VERIFY(defaultSampler != OpenGL::InvalidHandle, "Default sampler handle is invalid!");

        SCOPE_GUARD(glDeleteSamplers(1, &defaultSampler));

        // Read default parameters.
        glGetSamplerParameterfv(defaultSampler, GL_TEXTURE_BORDER_COLOR, &DefaultTextureBorderColor[0]);
        glGetSamplerParameteriv(defaultSampler, GL_TEXTURE_MIN_FILTER, &DefaultTextureMinFilter);
        glGetSamplerParameteriv(defaultSampler, GL_TEXTURE_MAG_FILTER, &DefaultTextureMagFilter);
        glGetSamplerParameteriv(defaultSampler, GL_TEXTURE_WRAP_S, &DefaultTextureWrapS);
        glGetSamplerParameteriv(defaultSampler, GL_TEXTURE_WRAP_T, &DefaultTextureWrapT);
        glGetSamplerParameteriv(defaultSampler, GL_TEXTURE_WRAP_R, &DefaultTextureWrapR);
        glGetSamplerParameterfv(defaultSampler, GL_TEXTURE_MIN_LOD, &DefaultTextureMinLOD);
        glGetSamplerParameterfv(defaultSampler, GL_TEXTURE_MAX_LOD, &DefaultTextureMaxLOD);
        glGetSamplerParameterfv(defaultSampler, GL_TEXTURE_LOD_BIAS, &DefaultTextureLODBias);
        glGetSamplerParameteriv(defaultSampler, GL_TEXTURE_COMPARE_MODE, &DefaultTextureCompareMode);
        glGetSamplerParameteriv(defaultSampler, GL_TEXTURE_COMPARE_FUNC, &DefaultTextureCompareFunc);
        glGetSamplerParameterfv(defaultSampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, &DefaultTextureMaxAniso);
        OpenGL::CheckErrors();

        // Finish initialization.
        DefaultsInitialized = true;
    }
}

SamplerInfo::SamplerInfo()
{
    // Initialize default values.
    InitializeDefaults();

    // Initialize member variables.
    textureBorderColor = DefaultTextureBorderColor;
    textureMinFilter = DefaultTextureMinFilter;
    textureMagFilter = DefaultTextureMagFilter;
    textureWrapS = DefaultTextureWrapS;
    textureWrapT = DefaultTextureWrapT;
    textureWrapR = DefaultTextureWrapR;
    textureMinLOD = DefaultTextureMinLOD;
    textureMaxLOD = DefaultTextureMaxLOD;
    textureLODBias = DefaultTextureLODBias;
    textureCompareMode = DefaultTextureCompareMode;
    textureCompareFunc = DefaultTextureCompareFunc;
    textureMaxAniso = DefaultTextureMaxAniso;
}

Sampler::~Sampler()
{
    this->DestroyHandle();
}

Sampler::Sampler(Sampler&& other) :
    Sampler()
{
    *this = std::move(other);
}

Sampler& Sampler::operator=(Sampler&& other)
{
    std::swap(m_renderContext, other.m_renderContext);
    std::swap(m_handle, other.m_handle);
    
    return *this;
}

void Sampler::DestroyHandle()
{
    // Release sampler handle.
    if(m_handle != OpenGL::InvalidHandle)
    {
        glDeleteSamplers(1, &m_handle);
        OpenGL::CheckErrors();

        m_handle = OpenGL::InvalidHandle;
    }
}

bool Sampler::Initialize(RenderContext* renderContext, const SamplerInfo& info)
{
    LOG("Creating sampler...");
    LOG_SCOPED_INDENT();

    // Check if handle has been already created.
    VERIFY(m_handle == OpenGL::InvalidHandle, "Sampler instance has been already initialized!");

    // Validate arguments.
    if(renderContext == nullptr)
    {
        LOG_ERROR("Invalid argument - \"renderContext\" is null!");
        return false;
    }

    // Setup a cleanup guard.
    bool initialized = false;

    // Create a sampler handle.
    SCOPE_GUARD_IF(!initialized, this->DestroyHandle());
    
    glGenSamplers(1, &m_handle);
    OpenGL::CheckErrors();

    if(m_handle == OpenGL::InvalidHandle)
    {
        LOG_ERROR("Sampler could not be created!");
        return false;
    }

    // Set sampling parameters.
    if(DefaultTextureBorderColor != info.textureBorderColor)
    {
        glSamplerParameterfv(m_handle, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(info.textureBorderColor));
    }

    if(DefaultTextureMinFilter != info.textureMinFilter)
    {
        glSamplerParameteri(m_handle, GL_TEXTURE_MIN_FILTER, info.textureMinFilter);
    }

    if(DefaultTextureMagFilter != info.textureMagFilter)
    {
        glSamplerParameteri(m_handle, GL_TEXTURE_MAG_FILTER, info.textureMagFilter);
    }

    if(DefaultTextureWrapS != info.textureWrapS)
    {
        glSamplerParameteri(m_handle, GL_TEXTURE_WRAP_S, info.textureWrapS);
    }

    if(DefaultTextureWrapT != info.textureWrapT)
    {
        glSamplerParameteri(m_handle, GL_TEXTURE_WRAP_T, info.textureWrapT);
    }

    if(DefaultTextureWrapR != info.textureWrapR)
    {
        glSamplerParameteri(m_handle, GL_TEXTURE_WRAP_R, info.textureWrapR);
    }

    if(DefaultTextureMinLOD != info.textureMinLOD)
    {
        glSamplerParameterf(m_handle, GL_TEXTURE_MIN_LOD, info.textureMinLOD);
    }

    if(DefaultTextureMaxLOD != info.textureMaxLOD)
    {
        glSamplerParameterf(m_handle, GL_TEXTURE_MAX_LOD, info.textureMaxLOD);
    }

    if(DefaultTextureLODBias != info.textureLODBias)
    {
        glSamplerParameterf(m_handle, GL_TEXTURE_LOD_BIAS, info.textureLODBias);
    }

    if(DefaultTextureCompareMode != info.textureCompareMode)
    {
        glSamplerParameteri(m_handle, GL_TEXTURE_COMPARE_MODE, info.textureCompareMode);
    }

    if(DefaultTextureCompareFunc != info.textureCompareFunc)
    {
        glSamplerParameteri(m_handle, GL_TEXTURE_COMPARE_FUNC, info.textureCompareFunc);
    }

    if(DefaultTextureMaxAniso != info.textureMaxAniso)
    {
        glSamplerParameterf(m_handle, GL_TEXTURE_MAX_ANISOTROPY_EXT, info.textureMaxAniso);
    }

    // Save render context reference.
    m_renderContext = renderContext;

    // Success!
    return initialized = true;
}

GLuint Sampler::GetHandle() const
{
    ASSERT(m_handle != OpenGL::InvalidHandle, "Sampler handle has not been created!");
    return m_handle;
}

bool Sampler::IsValid() const
{
    return m_handle != OpenGL::InvalidHandle;
}
