/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Graphics/RenderState.hpp"
using namespace Graphics;

bool Graphics::OpenGL::CheckErrors()
{
    bool errorFound = false;

    GLenum error = GL_NO_ERROR;
    while((error = glGetError()) != GL_NO_ERROR)
    {
        errorFound = true;

        #ifndef NDEBUG
            std::stringstream stream;
            stream << std::hex << std::setfill('0') << std::setw(4) << error;

            LOG_WARNING() << "Encountered OpenGL error with code 0x" << stream.str() << "!";
            ASSERT(error == GL_NO_ERROR, "Breaking due to encountered OpenGL error!");
        #endif
    }

    return errorFound != true;
}

RenderState::RenderState() :
    m_initialized(false)
{
    // glBindVertexArray
    m_vertexArrayBinding = OpenGL::InvalidHandle;

    // glBindBuffer
    for(GLuint& bufferBinding : m_bufferBindings)
    {
        bufferBinding = OpenGL::InvalidHandle;
    }

    // glActiveTexture
    m_activeTexture = GL_NONE;

    // glBindTexture
    for(GLuint& textureBinding : m_textureBindings)
    {
        textureBinding = OpenGL::InvalidHandle;
    }

    // glPixelStore
    for(GLint& pixelStore : m_pixelStore)
    {
        pixelStore = 0;
    }

    // glUseProgram
    m_currentProgram = OpenGL::InvalidHandle;

    // glViewport
    m_viewport = { 0, 0, 0, 0 };

    // glClearDeapth
    m_clearDepth = 0.0;

    // glClearColor
    m_clearColor = { 0.0f, 0.0f, 0.0f, 0.0f };
}

bool Graphics::RenderState::Initialize()
{
    LOG() << "Initializing a rendering state..." << LOG_INDENT();

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

    // glUseProgram
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*)&m_currentProgram);
    OpenGL::CheckErrors();

    // glViewport
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, &viewport[0]);
    OpenGL::CheckErrors();

    m_viewport = std::tie(viewport[0], viewport[1], viewport[2], viewport[3]);

    // glClearDeapth
    glGetDoublev(GL_DEPTH_CLEAR_VALUE, &m_clearDepth);
    OpenGL::CheckErrors();

    // glClearColor
    GLfloat clearColor[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, &clearColor[0]);
    OpenGL::CheckErrors();

    m_clearColor = std::tie(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);

    // Success!
    LOG_INFO() << "Success!";

    return m_initialized = true;
}

void RenderState::Apply(RenderState& other)
{
    ASSERT(m_initialized, "Render state is not initialized!");

    // glBindVertexArray
    BindVertexArray(other.m_vertexArrayBinding);

    // glBindBuffer
    for(int i = 0; i < OpenGL::BufferBindingTargetCount; ++i)
    {
        BindBuffer(std::get<0>(OpenGL::BufferBindingTargets[i]), other.m_bufferBindings[i]);
    }

    // glActiveTexture
    ActiveTexture(other.m_activeTexture);

    // glBindTexture
    for(int i = 0; i < OpenGL::TextureBindingTargetCount; ++i)
    {
        BindTexture(std::get<0>(OpenGL::TextureBindingTargets[i]), other.m_textureBindings[i]);
    }

    // glBindSampler
    ASSERT(m_samplerBindings.size() == other.m_samplerBindings.size(), "Different sampler binding array sizes between states!");

    for(size_t i = 0; i < m_samplerBindings.size(); ++i)
    {
        BindSampler(i, other.m_samplerBindings[i]);
    }

    // glPixelStore
    for(int i = 0; i < OpenGL::PixelStoreParameterCount; ++i)
    {
        PixelStore(OpenGL::PixelStoreParameters[i], other.m_pixelStore[i]);
    }

    // glUseProgram
    UseProgram(other.m_currentProgram);

    // glViewport
    Viewport(
        std::get<0>(other.m_viewport),
        std::get<1>(other.m_viewport),
        std::get<2>(other.m_viewport),
        std::get<3>(other.m_viewport)
    );

    // glClearDepth
    ClearDepth(other.m_clearDepth);

    // glClearColor
    ClearColor(
        std::get<0>(other.m_clearColor),
        std::get<1>(other.m_clearColor),
        std::get<2>(other.m_clearColor),
        std::get<3>(other.m_clearColor)
    );
}

void RenderState::BindVertexArray(GLuint array)
{
    ASSERT(m_initialized, "Render state is not initialized!");

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
    ASSERT(m_initialized, "Render state is not initialized!");

    return m_vertexArrayBinding;
}

void RenderState::BindBuffer(GLenum target, GLuint buffer)
{
    ASSERT(m_initialized, "Render state is not initialized!");
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
    ASSERT(m_initialized, "Render state is not initialized!");
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
    ASSERT(m_initialized, "Render state is not initialized!");
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
    ASSERT(m_initialized, "Render state is not initialized!");

    return m_activeTexture;
}

void RenderState::BindTexture(GLenum target, GLuint texture)
{
    ASSERT(m_initialized, "Render state is not initialized!");
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
    ASSERT(m_initialized, "Render state is not initialized!");
    ASSERT(target != OpenGL::InvalidEnum, "Unsupported texture binding target!");

    for(int i = 0; i < OpenGL::TextureBindingTargetCount; ++i)
    {
        if(std::get<0>(OpenGL::TextureBindingTargets[i]) == target)
            return m_textureBindings[i];
    }

    return OpenGL::InvalidHandle;
}

void RenderState::BindSampler(GLuint unit, GLuint sampler)
{
    ASSERT(m_initialized, "Render state is not initialized!");
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
    ASSERT(m_initialized, "Render state is not initialized!");
    ASSERT(!m_samplerBindings.empty(), "Sampler bindings array is empty!");
    VERIFY(unit >= 0 && unit < m_samplerBindings.size(), "Unsupported texture unit!");

    return m_samplerBindings[unit];
}

void RenderState::PixelStore(GLenum pname, GLint param)
{
    ASSERT(m_initialized, "Render state is not initialized!");
    ASSERT(pname != OpenGL::InvalidEnum, "Unsupported pixel store parameter!");

    // Check if states match.
    if(GetPixelStore(pname) == param)
        return;

    // Call OpenGL function.
    glPixelStorei(pname, param);
    OpenGL::CheckErrors();

    // Save changed state.
    for(int i = 0; i < OpenGL::PixelStoreParameterCount; ++i)
    {
        if(OpenGL::PixelStoreParameters[i] == pname)
            m_pixelStore[i] = param;
    }
}

GLint RenderState::GetPixelStore(GLenum pname) const
{
    ASSERT(m_initialized, "Render state is not initialized!");
    ASSERT(pname != OpenGL::InvalidEnum, "Unsupported pixel store parameter!");

    for(int i = 0; i < OpenGL::PixelStoreParameterCount; ++i)
    {
        if(OpenGL::PixelStoreParameters[i] == pname)
            return m_pixelStore[i];
    }

    return 0;
}

void RenderState::UseProgram(GLuint program)
{
    ASSERT(m_initialized, "Render state is not initialized!");
    
    // Check if state changed.
    if(GetCurrentProgram() == program)
        return;

    // Call OpenGL function.
    glUseProgram(program);
    OpenGL::CheckErrors();

    // Save changed state.
    m_currentProgram = program;
}

GLuint RenderState::GetCurrentProgram() const
{
    ASSERT(m_initialized, "Render state is not initialized!");

    return m_currentProgram;
}

void RenderState::Viewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    ASSERT(m_initialized, "Render state is not initialized!");

    // Check if state changed.
    if(GetViewport() == std::tie(x, y, width, height))
        return;

    // Call OpenGL function.
    glViewport(x, y, width, height);
    OpenGL::CheckErrors();

    // Save changed state.
    m_viewport = std::tie(x, y, width, height);
}

std::tuple<GLint, GLint, GLsizei, GLsizei> RenderState::GetViewport() const
{
    ASSERT(m_initialized, "Render state is not initialized!");

    return m_viewport;
}

void RenderState::ClearDepth(GLdouble depth)
{
    ASSERT(m_initialized, "Render state is not initialized!");
    
    // Check if state changed.
    if(GetClearDepth() == depth)
        return;

    // Call OpenGL function.
    glClearDepth(depth);
    OpenGL::CheckErrors();

    // Save changed state.
    m_clearDepth = depth;
}

GLdouble RenderState::GetClearDepth() const
{
    ASSERT(m_initialized, "Render state is not initialized!");
    
    return m_clearDepth;
}

void RenderState::ClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    ASSERT(m_initialized, "Render state is not initialized!");
    
    // Check if state changed.
    if(GetClearColor() == std::tie(red, green, blue, alpha))
        return;

    // Call OpenGL function.
    glClearColor(red, green, blue, alpha);
    OpenGL::CheckErrors();

    // Save changed state.
    m_clearColor = std::tie(red, green, blue, alpha);
}

std::tuple<GLfloat, GLfloat, GLfloat, GLfloat> RenderState::GetClearColor() const
{
    ASSERT(m_initialized, "Render state is not initialized!");
    
    return m_clearColor;
}

void RenderState::Clear(GLbitfield mask)
{
    ASSERT(m_initialized, "Render state is not initialized!");
    
    // Call OpenGL function.
    glClear(mask);
    OpenGL::CheckErrors();
}

void RenderState::DrawArrays(GLenum mode, GLint first, GLsizei count)
{
    ASSERT(m_initialized, "Render state is not initialized!");
    
    // Call OpenGL function.
    glDrawArrays(mode, first, count);
    OpenGL::CheckErrors();
}
