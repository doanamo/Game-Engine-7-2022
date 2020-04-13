/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Graphics/RenderContext.hpp"

namespace System
{
    class FileSystem;
}

/*
    Shader
    
    Loads and links GLSL shaders into an OpenGL program object.
    Supports geometry, vertex and fragment shaders.
    
    void ExampleGraphicsShader(Graphics::RenderContext* renderContext)
    {
        // Create a shader instance.
        ShaderLoadInfo info;
        info.filePath = "Data/Shader.glsl";

        Graphics::Shader shader;
        shader.Initialize(renderContext, info);

        // Use the created shader in our rendering pipeline.
        glUseProgram(shader.GetHandle());
        glUniformMatrix4fv(shader.GetUniformIndex("vertexTransform"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
    }

    ExampleShader.glsl
    [
        \#version 330
        
        #if defined(VERTEX_SHADER)
            uniform mat4 vertexTransform;
        
            layout(location = 0) in vec3 vertexPosition;
        
            void main()
            {
                gl_Position = vertexTransform * vec4(vertexPosition, 1.0f);
            }
        #endif
        
        #if defined(FRAGMENT_SHADER)
            out vec4 finalColor;
        
            void main()
            {
                finalColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
            }
        #endif
    ]
*/

namespace Graphics
{
    class RenderContext;

    class Shader : private NonCopyable
    {
    public:
        struct LoadFromString
        {
            System::FileSystem* fileSystem = nullptr;
            RenderContext* renderContext = nullptr;
            std::string shaderCode;
        };

        struct LoadFromFile
        {
            System::FileSystem* fileSystem = nullptr;
            RenderContext* renderContext = nullptr;
            std::string filePath;
        };

    public:
        Shader() = default;
        ~Shader();

        Shader(Shader&& other);
        Shader& operator=(Shader&& other);

        bool Initialize(const LoadFromString& params);
        bool Initialize(const LoadFromFile& params);

        template<typename Type>
        void SetUniform(std::string name, const Type& value);

        GLint GetAttributeIndex(std::string name) const;
        GLint GetUniformIndex(std::string name) const;
        GLuint GetHandle() const;
        bool IsValid() const;

    private:
        void DestroyHandle();

    private:
        RenderContext* m_renderContext = nullptr;
        GLuint m_handle = OpenGL::InvalidHandle;
    };

    using ShaderPtr = std::shared_ptr<Shader>;

    template<>
    inline void Shader::SetUniform(std::string name, const GLint& value)
    {
        VERIFY(m_handle != OpenGL::InvalidHandle);

        // Change shader program.
        GLuint previousProgram = m_renderContext->GetState().GetCurrentProgram();
        m_renderContext->GetState().UseProgram(GetHandle());

        SCOPE_GUARD(m_renderContext->GetState().UseProgram(previousProgram));

        // Set the uniform variable.
        glUniform1i(GetUniformIndex(name), value);
        OpenGL::CheckErrors();
    }

    template<>
    inline void Shader::SetUniform(std::string name, const glm::vec2& value)
    {
        VERIFY(m_handle != OpenGL::InvalidHandle);

        // Change shader program.
        GLuint previousProgram = m_renderContext->GetState().GetCurrentProgram();
        m_renderContext->GetState().UseProgram(GetHandle());

        SCOPE_GUARD(m_renderContext->GetState().UseProgram(previousProgram));

        // Set the uniform variable.
        glUniform2fv(GetUniformIndex(name), 1, glm::value_ptr(value));
        OpenGL::CheckErrors();
    }

    template<>
    inline void Shader::SetUniform(std::string name, const glm::mat4& value)
    {
        VERIFY(m_handle != OpenGL::InvalidHandle);

        // Change shader program.
        GLuint previousProgram = m_renderContext->GetState().GetCurrentProgram();
        m_renderContext->GetState().UseProgram(GetHandle());

        SCOPE_GUARD(m_renderContext->GetState().UseProgram(previousProgram));

        // Set the uniform variable.
        glUniformMatrix4fv(GetUniformIndex(name), 1, GL_FALSE, glm::value_ptr(value));
        OpenGL::CheckErrors();
    }
}
