/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Core/ServiceStorage.hpp>
#include "Graphics/RenderContext.hpp"

namespace System
{
    class FileHandle;
}

/*
    Shader
    
    Loads and links GLSL shaders into an OpenGL program object.
    Supports geometry, vertex and fragment shaders.
*/

namespace Graphics
{
    class RenderContext;

    class Shader final : private Common::NonCopyable
    {
    public:
        struct LoadFromString
        {
            const Core::ServiceStorage* services = nullptr;
            std::string shaderCode;
        };

        struct LoadFromFile
        {
            const Core::ServiceStorage* services = nullptr;
        };

        enum class CreateErrors
        {
            InvalidArgument,
            FailedFilePathResolve,
            InvalidFileContents,
            FailedShaderCreation,
            FailedShaderCompilation,
            FailedProgramCreation,
            FailedProgramLinkage,
        };

        using CreateResult = Common::Result<std::unique_ptr<Shader>, CreateErrors>;
        static CreateResult Create(const LoadFromString& params);
        static CreateResult Create(System::FileHandle& file, const LoadFromFile& params);

    public:
        ~Shader();

        template<typename Type>
        void SetUniform(std::string name, const Type& value);

        GLint GetAttributeIndex(std::string name) const;
        GLint GetUniformIndex(std::string name) const;
        GLuint GetHandle() const;

    private:
        Shader();

    private:
        RenderContext* m_renderContext = nullptr;
        GLuint m_handle = OpenGL::InvalidHandle;
    };

    using ShaderPtr = std::shared_ptr<Shader>;

    template<>
    inline void Shader::SetUniform(std::string name, const GLint& value)
    {
        // Change shader program.
        GLuint previousProgram = m_renderContext->GetState().GetCurrentProgram();
        m_renderContext->GetState().UseProgram(GetHandle());

        // Set uniform variable.
        glUniform1i(GetUniformIndex(name), value);
        OpenGL::CheckErrors();

        // Revert to previous program.
        m_renderContext->GetState().UseProgram(previousProgram);
    }

    template<>
    inline void Shader::SetUniform(std::string name, const glm::vec2& value)
    {
        // Change shader program.
        GLuint previousProgram = m_renderContext->GetState().GetCurrentProgram();
        m_renderContext->GetState().UseProgram(GetHandle());

        // Set uniform variable.
        glUniform2fv(GetUniformIndex(name), 1, glm::value_ptr(value));
        OpenGL::CheckErrors();

        // Revert to previous program.
        m_renderContext->GetState().UseProgram(previousProgram);
    }

    template<>
    inline void Shader::SetUniform(std::string name, const glm::mat4& value)
    {
        // Change shader program.
        GLuint previousProgram = m_renderContext->GetState().GetCurrentProgram();
        m_renderContext->GetState().UseProgram(GetHandle());

        // Set uniform variable.
        glUniformMatrix4fv(GetUniformIndex(name), 1, GL_FALSE, glm::value_ptr(value));
        OpenGL::CheckErrors();

        // Revert to previous program.
        m_renderContext->GetState().UseProgram(previousProgram);
    }
}
