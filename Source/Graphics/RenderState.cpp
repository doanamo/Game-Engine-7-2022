/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Graphics/RenderState.hpp"
using namespace Graphics;

bool OpenGL::CheckErrors()
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
    // glEnable
    for(GLboolean& capability : m_capabilities)
    {
        capability = GL_FALSE;
    }

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

    // glDepthMask
    m_depthMask = GL_TRUE;

    // glBlendFuncSeparate
    m_blendFuncSeparate = { GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO };

    // glBlendEquationSeparate
    m_blendEquationSeparate = { GL_ZERO, GL_ZERO };

    // glScissor
    m_scissorBox = { 0, 0, 0, 0 };
}

RenderState::~RenderState()
{
}

RenderState::RenderState(RenderState&& other) :
    RenderState()
{
    // Call the move assignment.
    *this = std::move(other);
}

RenderState& RenderState::operator=(RenderState&& other)
{
    // Swap class members.
    std::swap(m_initialized, other.m_initialized);
    std::swap(m_capabilities, other.m_capabilities);
    std::swap(m_vertexArrayBinding, other.m_vertexArrayBinding);
    std::swap(m_bufferBindings, other.m_bufferBindings);
    std::swap(m_activeTexture, other.m_activeTexture);
    std::swap(m_textureBindings, other.m_textureBindings);
    std::swap(m_samplerBindings, other.m_samplerBindings);
    std::swap(m_pixelStore, other.m_pixelStore);
    std::swap(m_currentProgram, other.m_currentProgram);
    std::swap(m_viewport, other.m_viewport);
    std::swap(m_clearDepth, other.m_clearDepth);
    std::swap(m_clearColor, other.m_clearColor);
    std::swap(m_depthMask, other.m_depthMask);
    std::swap(m_blendFuncSeparate, other.m_blendFuncSeparate);
    std::swap(m_blendEquationSeparate, other.m_blendEquationSeparate);
    std::swap(m_scissorBox, other.m_scissorBox);

    return *this;
}

bool RenderState::Initialize()
{
    LOG() << "Initializing rendering state..." << LOG_INDENT();

    // glEnable
    for(int i = 0; i < OpenGL::CapabilityCount; ++i)
    {
        m_capabilities[i] = glIsEnabled(OpenGL::Capabilities[i]);
        OpenGL::CheckErrors();
    }

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
    OpenGL::CheckErrors();

    m_samplerBindings.resize(SamplerBindingUnitCount, OpenGL::InvalidHandle);

    for(std::size_t i = 0; i < m_samplerBindings.size(); ++i)
    {
        glActiveTexture(Utility::NumericalCast<GLenum>(GL_TEXTURE0 + i));
        glGetIntegerv(GL_SAMPLER_BINDING, (GLint*)&m_samplerBindings[i]);
        OpenGL::CheckErrors();
    }

    glActiveTexture(m_activeTexture);
    OpenGL::CheckErrors();

    // glPixelStore
    for(std::size_t i = 0; i < OpenGL::PixelStoreParameterCount; ++i)
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

    // glDepthMask
    glGetBooleanv(GL_DEPTH_WRITEMASK, &m_depthMask);
    OpenGL::CheckErrors();

    // glBlendFuncSeparate
    GLenum blendSrcRGB;
    glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&blendSrcRGB);
    OpenGL::CheckErrors();

    GLenum blendDstRGB;
    glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&blendDstRGB);
    OpenGL::CheckErrors();

    GLenum blendSrcAlpha;
    glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&blendSrcAlpha);
    OpenGL::CheckErrors();

    GLenum blendDstAlpha;
    glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&blendDstAlpha);
    OpenGL::CheckErrors();

    m_blendFuncSeparate = std::tie(blendSrcRGB, blendDstRGB, blendSrcAlpha, blendDstAlpha);

    // glBlendEquationSeparate
    GLenum blendEquationRGB;
    glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&blendEquationRGB);
    OpenGL::CheckErrors();

    GLenum blendEquationAlpha;
    glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&blendEquationAlpha);
    OpenGL::CheckErrors();

    m_blendEquationSeparate = std::tie(blendEquationRGB, blendEquationAlpha);

    // glScissor
    GLint scissorBox[4];
    glGetIntegerv(GL_SCISSOR_BOX, &scissorBox[0]);
    OpenGL::CheckErrors();

    m_scissorBox = std::tie(scissorBox[0], scissorBox[1], scissorBox[2], scissorBox[3]);

    // Success!
    return m_initialized = true;
}

void RenderState::Apply(RenderState& other)
{
    ASSERT(m_initialized, "Render state is not initialized!");

    // glEnable
    for(std::size_t i = 0; i < OpenGL::CapabilityCount; ++i)
    {
        if(other.m_capabilities[i] == GL_TRUE)
        {
            this->Enable(OpenGL::Capabilities[i]);
        }
        else
        {
            this->Disable(OpenGL::Capabilities[i]);
        }
    }

    // glBindVertexArray
    this->BindVertexArray(other.m_vertexArrayBinding);

    // glBindBuffer
    for(std::size_t i = 0; i < OpenGL::BufferBindingTargetCount; ++i)
    {
        this->BindBuffer(std::get<0>(OpenGL::BufferBindingTargets[i]), other.m_bufferBindings[i]);
    }

    // glActiveTexture
    this->ActiveTexture(other.m_activeTexture);

    // glBindTexture
    for(int i = 0; i < OpenGL::TextureBindingTargetCount; ++i)
    {
        this->BindTexture(std::get<0>(OpenGL::TextureBindingTargets[i]), other.m_textureBindings[i]);
    }

    // glBindSampler
    ASSERT(m_samplerBindings.size() == other.m_samplerBindings.size(), 
        "Different sampler binding array sizes between states!");

    for(std::size_t i = 0; i < m_samplerBindings.size(); ++i)
    {
        this->BindSampler(Utility::NumericalCast<GLuint>(i), other.m_samplerBindings[i]);
    }

    // glPixelStore
    for(std::size_t i = 0; i < OpenGL::PixelStoreParameterCount; ++i)
    {
        this->PixelStore(OpenGL::PixelStoreParameters[i], other.m_pixelStore[i]);
    }

    // glUseProgram
    this->UseProgram(other.m_currentProgram);

    // glViewport
    this->Viewport(
        std::get<0>(other.m_viewport),
        std::get<1>(other.m_viewport),
        std::get<2>(other.m_viewport),
        std::get<3>(other.m_viewport)
    );

    // glClearDepth
    this->ClearDepth(other.m_clearDepth);

    // glClearColor
    this->ClearColor(
        std::get<0>(other.m_clearColor),
        std::get<1>(other.m_clearColor),
        std::get<2>(other.m_clearColor),
        std::get<3>(other.m_clearColor)
    );

    // glDepthMask
    this->DepthMask(other.m_depthMask);

    // glBlendFuncSeparate
    this->BlendFuncSeparate(
        std::get<0>(other.m_blendFuncSeparate),
        std::get<1>(other.m_blendFuncSeparate),
        std::get<2>(other.m_blendFuncSeparate),
        std::get<3>(other.m_blendFuncSeparate)
    );

    // glBlendEquationSeparate
    this->BlendEquationSeparate(
        std::get<0>(other.m_blendEquationSeparate),
        std::get<1>(other.m_blendEquationSeparate)
    );

    // glScissor
    this->Scissor(
        std::get<0>(other.m_scissorBox),
        std::get<1>(other.m_scissorBox),
        std::get<2>(other.m_scissorBox),
        std::get<3>(other.m_scissorBox)
    );
}

void RenderState::Enable(GLenum cap)
{
    ASSERT(m_initialized, "Render state is not initialized!");

    // Check if states match.
    if(this->IsEnabled(cap))
        return;

    // Call OpenGL function.
    glEnable(cap);
    OpenGL::CheckErrors();

    // Save changed state.
    for(int i = 0; i < OpenGL::CapabilityCount; ++i)
    {
        if(OpenGL::Capabilities[i] == cap)
        {
            m_capabilities[i] = GL_TRUE;
            break;
        }
    }
}

void RenderState::Disable(GLenum cap)
{
    ASSERT(m_initialized, "Render state is not initialized!");

    // Check if states match.
    if(!IsEnabled(cap))
        return;

    // Call OpenGL function.
    glDisable(cap);
    OpenGL::CheckErrors();

    // Save changed state.
    for(int i = 0; i < OpenGL::CapabilityCount; ++i)
    {
        if(OpenGL::Capabilities[i] == cap)
        {
            m_capabilities[i] = GL_FALSE;
            break;
        }
    }
}

GLboolean RenderState::IsEnabled(GLenum cap) const
{
    ASSERT(m_initialized, "Render state is not initialized!");

    for(int i = 0; i < OpenGL::CapabilityCount; ++i)
    {
        if(OpenGL::Capabilities[i] == cap)
            return m_capabilities[i];
    }

    ASSERT(false, "Unsupported capability!");

    return GL_FALSE;
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
        {
            m_bufferBindings[i] = buffer;
            break;
        }
    }
}

GLuint RenderState::GetBufferBinding(GLenum target) const
{
    ASSERT(m_initialized, "Render state is not initialized!");

    for(int i = 0; i < OpenGL::BufferBindingTargetCount; ++i)
    {
        if(std::get<0>(OpenGL::BufferBindingTargets[i]) == target)
            return m_bufferBindings[i];
    }

    ASSERT(false, "Unsupported buffer binding target!");

    return OpenGL::InvalidHandle;
}

void RenderState::ActiveTexture(GLenum texture)
{
    ASSERT(m_initialized, "Render state is not initialized!");

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
        {
            m_textureBindings[i] = texture;
            break;
        }
    }
}

GLuint RenderState::GetTextureBinding(GLenum target) const
{
    ASSERT(m_initialized, "Render state is not initialized!");
    
    for(int i = 0; i < OpenGL::TextureBindingTargetCount; ++i)
    {
        if(std::get<0>(OpenGL::TextureBindingTargets[i]) == target)
            return m_textureBindings[i];
    }

    ASSERT(false, "Unsupported texture binding target!");

    return OpenGL::InvalidHandle;
}

void RenderState::BindSampler(GLuint unit, GLuint sampler)
{
    ASSERT(m_initialized, "Render state is not initialized!");

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
        {
            m_pixelStore[i] = param;
            break;
        }
    }
}

GLint RenderState::GetPixelStore(GLenum pname) const
{
    ASSERT(m_initialized, "Render state is not initialized!");

    for(int i = 0; i < OpenGL::PixelStoreParameterCount; ++i)
    {
        if(OpenGL::PixelStoreParameters[i] == pname)
            return m_pixelStore[i];
    }

    ASSERT(false, "Unsupported pixel store parameter!");

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

void RenderState::DepthMask(GLboolean flag)
{
    ASSERT(m_initialized, "Render state is not initialized!");

    // Check if state will changed.
    if(GetDepthMask() == flag)
        return;

    // Call OpenGL function.
    glDepthMask(flag);
    OpenGL::CheckErrors();

    // Save changed state.
    m_depthMask = flag;
}

GLboolean RenderState::GetDepthMask() const
{
    ASSERT(m_initialized, "Render state is not initialized!");

    return m_depthMask;
}

void RenderState::BlendFunc(GLenum sfactor, GLenum dfactor)
{
    ASSERT(m_initialized, "Render state is not initialized!");

    // Call aliased OpenGL function.
    this->BlendFuncSeparate(sfactor, dfactor, sfactor, dfactor);
}

void RenderState::BlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
    ASSERT(m_initialized, "Render state is not initialized!");

    // Check if state changed.
    if(GetBlendFuncSeparate() == std::tie(srcRGB, dstRGB, srcAlpha, dstAlpha))
        return;

    // Call OpenGL function.
    glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
    OpenGL::CheckErrors();

    // Save changed state.
    m_blendFuncSeparate = std::tie(srcRGB, dstRGB, srcAlpha, dstAlpha);
}

std::tuple<GLenum, GLenum, GLenum, GLenum> RenderState::GetBlendFuncSeparate() const
{
    ASSERT(m_initialized, "Render state is not initialized!");

    return m_blendFuncSeparate;
}

void RenderState::BlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)
{
    ASSERT(m_initialized, "Render state is not initialized!");

    // Check if state changed.
    if(GetBlendEquationSeperate() == std::tie(modeRGB, modeAlpha))
        return;

    // Call OpenGL function.
    glBlendEquationSeparate(modeRGB, modeAlpha);
    OpenGL::CheckErrors();

    // Save changed state.
    m_blendEquationSeparate = std::tie(modeRGB, modeAlpha);
}

std::tuple<GLenum, GLenum> RenderState::GetBlendEquationSeperate() const
{
    ASSERT(m_initialized, "Render state is not initialized!");

    return m_blendEquationSeparate;
}

void RenderState::Scissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
    ASSERT(m_initialized, "Render state is not initialized!");

    // Check if state changed.
    if(GetScissorBox() == std::tie(x, y, width, height))
        return;

    // Call OpenGL function.
    glScissor(x, y, width, height);
    OpenGL::CheckErrors();

    // Save changed state.
    m_scissorBox = std::tie(x, y, width, height);
}

std::tuple<GLint, GLint, GLsizei, GLsizei> RenderState::GetScissorBox() const
{
    ASSERT(m_initialized, "Render state is not initialized!");

    return m_scissorBox;
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

void RenderState::DrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices)
{
    ASSERT(m_initialized, "Render state is not initialized!");

    // Call OpenGL function.
    glDrawElements(mode, count, type, indices);
    OpenGL::CheckErrors();
}
