/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Graphics/Sampler.hpp"
#include "Graphics/RenderContext.hpp"
using namespace Graphics;

SamplerInfo::SamplerInfo() :
    textureBorderColor(0.0f, 0.0f, 0.0f, 0.0f),
    textureMinFilter(GL_NEAREST_MIPMAP_LINEAR),
    textureMagFilter(GL_LINEAR),
    textureWrapS(GL_REPEAT),
    textureWrapT(GL_REPEAT),
    textureWrapR(GL_REPEAT),
    textureMinLOD(-1000.0f),
    textureMaxLOD(1000.0f),
    textureLODBias(0.0f),
    textureCompareMode(GL_NONE),
    textureCompareFunc(GL_LEQUAL),
    textureMaxAniso(1.0f)   
{
}

Graphics::Sampler::Sampler(RenderContext* renderContext) :
    m_renderContext(renderContext),
    m_handle(OpenGL::InvalidHandle)
{
}

Sampler::~Sampler()
{
    this->DestroyHandle();
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

bool Sampler::Create(const SamplerInfo& info)
{
    LOG() << "Creating sampler..." << LOG_INDENT();

    // Check if handle has been already created.
    VERIFY(m_handle == OpenGL::InvalidHandle, "Sampler instance has been already initialized!");

    // Setup a cleanup guard.
    bool initialized = false;

    // Create a sampler handle.
    SCOPE_GUARD_IF(!initialized, this->DestroyHandle());
    
    glGenSamplers(1, &m_handle);
    OpenGL::CheckErrors();

    if(m_handle == OpenGL::InvalidHandle)
    {
        LOG_ERROR() << "Could not create a sampler!";
        return false;
    }

    // Set sampling parameters.
    glSamplerParameterfv(m_handle, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(info.textureBorderColor));
    glSamplerParameteri(m_handle, GL_TEXTURE_MIN_FILTER, info.textureMinFilter);
    glSamplerParameteri(m_handle, GL_TEXTURE_MAG_FILTER, info.textureMagFilter);
    glSamplerParameteri(m_handle, GL_TEXTURE_WRAP_S, info.textureWrapS);
    glSamplerParameteri(m_handle, GL_TEXTURE_WRAP_T, info.textureWrapT);
    glSamplerParameteri(m_handle, GL_TEXTURE_WRAP_R, info.textureWrapR);
    glSamplerParameterf(m_handle, GL_TEXTURE_MIN_LOD, info.textureMinLOD);
    glSamplerParameterf(m_handle, GL_TEXTURE_MAX_LOD, info.textureMaxLOD);
    glSamplerParameterf(m_handle, GL_TEXTURE_LOD_BIAS, info.textureLODBias);
    glSamplerParameteri(m_handle, GL_TEXTURE_COMPARE_MODE, info.textureCompareMode);
    glSamplerParameteri(m_handle, GL_TEXTURE_COMPARE_FUNC, info.textureCompareFunc);
    glSamplerParameterf(m_handle, GL_TEXTURE_MAX_ANISOTROPY_EXT, info.textureMaxAniso);

    // Success!
    LOG_INFO() << "Success!";

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
