/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Graphics/Precompiled.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/RenderContext.hpp"
#include <System/FileSystem/FileHandle.hpp>
using namespace Graphics;

namespace
{
    struct ShaderType
    {
        const char* name;
        const char* define;
        GLenum type;
    };

    const ShaderType ShaderTypes[] =
    {
        { "vertex shader",   "VERTEX_SHADER",   GL_VERTEX_SHADER   },
        { "fragment shader", "FRAGMENT_SHADER", GL_FRAGMENT_SHADER },
    };

    const int ShaderTypeCount = Common::StaticArraySize(ShaderTypes);
}

Shader::Shader() = default;
Shader::~Shader()
{
    if(m_handle != OpenGL::InvalidHandle)
    {
        glDeleteProgram(m_handle);
        OpenGL::CheckErrors();
    }
}

Shader::CreateResult Shader::Create(const LoadFromString& params)
{
    LOG_PROFILE_SCOPE("Create shader");

    // Check arguments.
    CHECK_ARGUMENT_OR_RETURN(params.renderContext,
        Common::Failure(CreateErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(!params.shaderCode.empty(),
        Common::Failure(CreateErrors::InvalidArgument));

    // Create instance.
    auto instance = std::unique_ptr<Shader>(new Shader());
    instance->m_renderContext = params.renderContext;

    // Create array of shader objects for each type that can be linked.
    GLuint shaderObjects[ShaderTypeCount] = { 0 };
    SCOPE_GUARD([&shaderObjects]
    {
        for(GLuint shaderObject : shaderObjects)
        {
            // Delete shaders after we link them into a program.
            glDeleteShader(shaderObject);
        }
    });

    // Create mutable copy of the provided shader code.
    std::string shaderCode = params.shaderCode;

    // Extract shader version.
    std::string shaderVersion;
    std::size_t versionStart = shaderCode.find("#version ");
    std::size_t versionEnd = shaderCode.find('\n', versionStart);

    if(versionStart != std::string::npos)
    {
        shaderVersion = shaderCode.substr(versionStart, versionEnd - versionStart + 1);
        shaderCode.erase(versionStart, versionEnd + 1);
    }

    // Compile shader objects.
    bool shaderObjectsFound = false;
    for(unsigned int i = 0; i < ShaderTypeCount; ++i)
    {
        const ShaderType& shaderType = ShaderTypes[i];
        GLuint& shaderObject = shaderObjects[i];

        // Compile shader object if found.
        if(shaderCode.find(shaderType.define) != std::string::npos)
        {
            shaderObjectsFound = true;

            LOG_PROFILE_SCOPE("Compile {}", shaderType.name);

            // Create shader object.
            shaderObject = glCreateShader(shaderType.type);
            OpenGL::CheckErrors();

            if(shaderObject == OpenGL::InvalidHandle)
            {
                LOG_ERROR("Shader object could not be created!");
                return Common::Failure(CreateErrors::FailedShaderCreation);
            }

            // Prepare preprocessor define.
            std::string shaderDefine = "#define ";
            shaderDefine += shaderType.define;
            shaderDefine += "\n";

            // Compile shader object code.
            const char* shaderCodeSegments[] =
            {
                shaderVersion.c_str(),
                shaderDefine.c_str(),
                shaderCode.c_str(),
            };

            const std::size_t shaderCodeSegmentCount = Common::StaticArraySize(shaderCodeSegments);

            glShaderSource(shaderObject,
                Common::NumericalCast<GLsizei>(shaderCodeSegmentCount),
                (const GLchar**)&shaderCodeSegments, nullptr);
            OpenGL::CheckErrors();

            glCompileShader(shaderObject);
            OpenGL::CheckErrors();

            // Check compiling info.
            GLint infoLength = 0;
            glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &infoLength);
            OpenGL::CheckErrors();

            if(infoLength > 1)
            {
                std::vector<char> infoText(infoLength);
                glGetShaderInfoLog(shaderObject, infoLength, &infoLength, &infoText[0]);
                OpenGL::CheckErrors();

                LOG_ERROR("Shader compilation error output:");
                LOG_ERROR("{}", infoText.data());
            }

            // Check compiling results.
            GLint compileStatus = 0;
            glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &compileStatus);
            OpenGL::CheckErrors();

            if(compileStatus == GL_FALSE)
            {
                LOG_ERROR("Shader object could not be compiled!");
                return Common::Failure(CreateErrors::FailedShaderCompilation);
            }
        }
    }

    // Check if any shader objects were found.
    if(shaderObjectsFound == false)
    {
        LOG_ERROR("Could not find any shader objects!");
        return Common::Failure(CreateErrors::FailedShaderCompilation);
    }

    // Create shader program.
    instance->m_handle = glCreateProgram();
    OpenGL::CheckErrors();

    if(instance->m_handle == OpenGL::InvalidHandle)
    {
        LOG_ERROR("Shader program could not be created!");
        return Common::Failure(CreateErrors::FailedProgramCreation);
    }

    // Attach compiled shader objects.
    for(unsigned int & shaderObject : shaderObjects)
    {
        if(shaderObject != OpenGL::InvalidHandle)
        {
            glAttachShader(instance->m_handle, shaderObject);
            OpenGL::CheckErrors();
        }
    }

    // Link attached shader objects.
    {
        LOG_PROFILE_SCOPE("Link shader");

        // Create link result handle.
        glLinkProgram(instance->m_handle);
        OpenGL::CheckErrors();

        // Detach already linked shader objects.
        for(unsigned int & shaderObject : shaderObjects)
        {
            if(shaderObject != OpenGL::InvalidHandle)
            {
                glDetachShader(instance->m_handle, shaderObject);
                OpenGL::CheckErrors();
            }
        }
    }

    // Check linking output.
    GLint infoLength = 0;
    glGetProgramiv(instance->m_handle, GL_INFO_LOG_LENGTH, &infoLength);
    OpenGL::CheckErrors();

    if(infoLength > 1)
    {
        std::vector<char> infoText(infoLength);
        glGetProgramInfoLog(instance->m_handle, infoLength, &infoLength, &infoText[0]);
        OpenGL::CheckErrors();

        LOG_ERROR("Shader linkage error output:");
        LOG_ERROR("{}", infoText.data());
    }

    // Check linking results.
    GLint linkStatus = 0;
    glGetProgramiv(instance->m_handle, GL_LINK_STATUS, &linkStatus);
    OpenGL::CheckErrors();

    if(linkStatus == GL_FALSE)
    {
        LOG_ERROR("Shader program could not be linked!");
        return Common::Failure(CreateErrors::FailedProgramLinkage);
    }

    return Common::Success(std::move(instance));
}

Shader::CreateResult Shader::Create(System::FileHandle& file, const LoadFromFile& params)
{
    LOG_PROFILE_SCOPE("Load shader from \"{}\" file",
        file.GetPath().generic_string());

    LOG_INFO("Loading shader from \"{}\" file...",
        file.GetPath().generic_string());

    // Validate arguments.
    CHECK_ARGUMENT_OR_RETURN(params.renderContext,
        Common::Failure(CreateErrors::InvalidArgument));

    // Load shader code from a file.
    std::string shaderCode = file.ReadAsTextString();
    if(shaderCode.empty())
    {
        LOG_ERROR("Shader file could not be read!");
        return Common::Failure(CreateErrors::InvalidFileContents);
    }

    // Create instance.
    LoadFromString compileParams;
    compileParams.renderContext = params.renderContext;
    compileParams.shaderCode = std::move(shaderCode);
    return Create(compileParams);
}

GLint Shader::GetAttributeIndex(std::string name) const
{
    ASSERT(!name.empty(), "Attribute name cannot be empty!");
    GLint location = glGetAttribLocation(m_handle, name.c_str());
    return location;
}

GLint Shader::GetUniformIndex(std::string name) const
{
    ASSERT(!name.empty(), "Uniform name cannot be empty!");
    GLint location = glGetUniformLocation(m_handle, name.c_str());
    return location;
}
