/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Graphics/RenderState.hpp"
using namespace Graphics;

void OpenGL::CheckErrors()
{
    #ifndef NDEBUG
        GLenum error;
        while((error = glGetError()) != GL_NO_ERROR)
        {
            std::stringstream stream;
            stream << std::hex << std::setfill('0') << std::setw(4) << error;

            LOG_WARNING() << "Encountered OpenGL error with code 0x" << stream.str() << "!";
            ASSERT(error != GL_NO_ERROR, "Breaking due to encountered OpenGL error!");
        }
    #endif
}

RenderState::RenderState()
{
    // glBindVertexArray
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, (GLint*)&m_vertexArrayBinding);
    OpenGL::CheckErrors();

    // glBindBuffer
    for(int i = 0; i < OpenGL::BufferBindingTargetCount; ++i)
    {
        glGetIntegerv(std::get<1>(OpenGL::BufferBindingTargets[i]), (GLint*)&m_bufferBindings[i]);
        OpenGL::CheckErrors();
    }

    // glActiveTexture
    glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&m_activeTexture);
    OpenGL::CheckErrors();

    // glBindTexture
    for(int i = 0; i < OpenGL::TextureBindingTargetCount; ++i)
    {
        glGetIntegerv(std::get<1>(OpenGL::TextureBindingTargets[i]), (GLint*)&m_textureBindings[i]);
        OpenGL::CheckErrors();
    }

    // glBindSampler
    int SamplerBindingUnitCount = 0;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &SamplerBindingUnitCount);
    m_samplerBindings.resize(SamplerBindingUnitCount, OpenGL::InvalidHandle);

    for(size_t i = 0; i < m_samplerBindings.size(); ++i)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glGetIntegerv(GL_SAMPLER_BINDING, (GLint*)&m_samplerBindings[i]);
        OpenGL::CheckErrors();
    }

    glActiveTexture(m_activeTexture);
    OpenGL::CheckErrors();

    // glPixelStore
    for(int i = 0; i < OpenGL::PixelStoreParameterCount; ++i)
    {
        glGetIntegerv(OpenGL::PixelStoreParameters[i], &m_pixelStore[i]);
        OpenGL::CheckErrors();
    }
}

void RenderState::BindVertexArray(GLuint array)
{
    // Check if states match.
    if(GetVertexArrayBinding() == array)
        return;

    // Call OpenGL function.
    glBindVertexArray(array);
    OpenGL::CheckErrors();

    // Save changed state.
    m_vertexArrayBinding = array;
}

GLuint RenderState::GetVertexArrayBinding() const
{
    return m_vertexArrayBinding;
}

void RenderState::BindBuffer(GLenum target, GLuint buffer)
{
    ASSERT(target != OpenGL::InvalidEnum, "Unsupported buffer binding target!");

    // Check if states match.
    if(GetBufferBinding(target) == buffer)
        return;

    // Call OpenGL function.
    glBindBuffer(target, buffer);
    OpenGL::CheckErrors();

    // Save changed state.
    for(int i = 0; i < OpenGL::BufferBindingTargetCount; ++i)
    {
        if(std::get<0>(OpenGL::BufferBindingTargets[i]) == target)
            m_bufferBindings[i] = buffer;
    }
}

GLuint RenderState::GetBufferBinding(GLenum target) const
{
    ASSERT(target != OpenGL::InvalidEnum, "Unsupported buffer binding target!");

    for(int i = 0; i < OpenGL::BufferBindingTargetCount; ++i)
    {
        if(std::get<0>(OpenGL::BufferBindingTargets[i]) == target)
            return m_bufferBindings[i];
    }

    return OpenGL::InvalidHandle;
}

void RenderState::ActiveTexture(GLenum texture)
{
    ASSERT(texture >= GL_TEXTURE0 && texture < GL_TEXTURE0 + m_samplerBindings.size(), "Unsupported texture unit!");

    // Check if states match.
    if(GetActiveTexture() == texture)
        return;

    // Call OpenGL function.
    glActiveTexture(texture);
    OpenGL::CheckErrors();

    // Save changed state.
    m_activeTexture = texture;
}

GLenum RenderState::GetActiveTexture() const
{
    return m_activeTexture;
}

void RenderState::BindTexture(GLenum target, GLuint texture)
{
    ASSERT(target != OpenGL::InvalidEnum, "Unsupported texture binding target!");

    // Check if states match.
    if(GetTextureBinding(target) == texture)
        return;

    // Call OpenGL function.
    glBindTexture(target, texture);
    OpenGL::CheckErrors();

    // Save changed state.
    for(int i = 0; i < OpenGL::TextureBindingTargetCount; ++i)
    {
        if(std::get<0>(OpenGL::TextureBindingTargets[i]) == target)
            m_textureBindings[i] = texture;
    }
}

GLuint RenderState::GetTextureBinding(GLenum target) const
{
    ASSERT(target != OpenGL::InvalidEnum, "Unsupported texture binding target!");

    for(int i = 1; i < OpenGL::TextureBindingTargetCount; ++i)
    {
        if(std::get<0>(OpenGL::TextureBindingTargets[i]) == target)
            return m_textureBindings[i];
    }

    return OpenGL::InvalidHandle;
}

void RenderState::BindSampler(GLuint unit, GLuint sampler)
{
    ASSERT(!m_samplerBindings.empty(), "Sampler bindings array is empty!");
    VERIFY(unit >= 0 && unit < m_samplerBindings.size(), "Unsupported texture unit!");

    // Check if states match.
    if(GetSamplerBinding(unit) == sampler)
        return;

    // Call OpenGL function.
    glBindSampler(unit, sampler);
    OpenGL::CheckErrors();

    // Save changed state.
    m_samplerBindings[unit] = sampler;
}

GLuint RenderState::GetSamplerBinding(GLuint unit) const
{
    ASSERT(!m_samplerBindings.empty(), "Sampler bindings array is empty!");
    VERIFY(unit >= 0 && unit < m_samplerBindings.size(), "Unsupported texture unit!");

    return m_samplerBindings[unit];
}

void RenderState::PixelStore(GLenum pname, GLint param)
{
    ASSERT(pname != OpenGL::InvalidEnum, "Unsupported pixel store parameter!");

    // Check if states match.
    if(GetTextureBinding(pname) == param)
        return;

    // Call OpenGL function.
    glPixelStorei(pname, param);
    OpenGL::CheckErrors();

    // Save changed state.
    for(int i = 1; i < OpenGL::PixelStoreParameterCount; ++i)
    {
        if(OpenGL::PixelStoreParameters[i] == pname)
            m_pixelStore[i] = param;
    }
}

GLint RenderState::GetPixelStore(GLenum pname) const
{
    ASSERT(pname != OpenGL::InvalidEnum, "Unsupported pixel store parameter!");

    for(int i = 1; i < OpenGL::PixelStoreParameterCount; ++i)
    {
        if(OpenGL::PixelStoreParameters[i] == pname)
            return m_pixelStore[i];
    }

    return m_pixelStore[0];
}
