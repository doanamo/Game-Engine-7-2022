/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/RenderContext.hpp"
#include "System/FileSystem.hpp"
#include "Engine/Root.hpp"
using namespace Graphics;

namespace
{
    // Available shader types.
    struct ShaderType
    {
        const char* name;
        const char* define;
        GLenum type;
    };

    const int ShaderTypeCount = 3;
    const ShaderType ShaderTypes[ShaderTypeCount] =
    {
        { "vertex shader",   "VERTEX_SHADER",   GL_VERTEX_SHADER   },
        { "geometry shader", "GEOMETRY_SHADER", GL_GEOMETRY_SHADER },
        { "fragment shader", "FRAGMENT_SHADER", GL_FRAGMENT_SHADER },
    };
}

Shader::LoadFromString::LoadFromString() :
    engine(nullptr)
{
}

Shader::LoadFromFile::LoadFromFile() :
    engine(nullptr)
{
}

Shader::Shader() :
    m_renderContext(nullptr),
    m_handle(OpenGL::InvalidHandle)
{
}

Shader::~Shader()
{
    this->DestroyHandle();
}

Shader::Shader(Shader&& other) :
    Shader()
{
    // Call the move assignment.
    *this = std::move(other);
}

Shader& Shader::operator=(Shader&& other)
{
    // Swap class members.
    std::swap(m_renderContext, other.m_renderContext);
    std::swap(m_handle, other.m_handle);

    return *this;
}

void Shader::DestroyHandle()
{
    // Release the program handle.
    if(m_handle != OpenGL::InvalidHandle)
    {
        glDeleteProgram(m_handle);
        OpenGL::CheckErrors();

        m_handle = OpenGL::InvalidHandle;
    }
}

bool Shader::Initialize(const LoadFromString& params)
{
    LOG() << "Compiling shader code..." << LOG_INDENT();

    // Check if handle has been already created.
    VERIFY(m_handle == OpenGL::InvalidHandle, "Shader instance has been already initialized!");

    // Setup a cleanup guard.
    bool initialized = false;
    
    SCOPE_GUARD_IF(!initialized, *this = Shader());

    // Validate arguments.
    if(params.engine == nullptr)
    {
        LOG_ERROR() << "Invalid parameter - \"engine\" is null!";
        return false;
    }

    if(params.shaderCode.empty())
    {
        LOG_ERROR() << "Invalid argument - \"params.shaderCode\" is empty!";
        return false;
    }

    m_renderContext = &params.engine->GetRenderContext();

    // Create an array of shader objects for each type that can be linked.
    GLuint shaderObjects[ShaderTypeCount] = { 0 };

    SCOPE_GUARD_BEGIN();
    {
        for(int i = 0; i < ShaderTypeCount; ++i)
        {
            // Delete shaders after we link them into a program.
            glDeleteShader(shaderObjects[i]);
        }
    }
    SCOPE_GUARD_END();

    // Create a mutable copy of the provided shader code.
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

            LOG_INFO() << "Compiling " << shaderType.name << "...";

            // Create a shader object.
            shaderObject = glCreateShader(shaderType.type);
            OpenGL::CheckErrors();

            if(shaderObject == OpenGL::InvalidHandle)
            {
                LOG_ERROR() << "Shader object could not be created!";
                return false;
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

            const std::size_t shaderCodeSegmentCount = Utility::StaticArraySize(shaderCodeSegments);

            glShaderSource(shaderObject,
                Utility::NumericalCast<GLsizei>(shaderCodeSegmentCount),
                (const GLchar**)&shaderCodeSegments, nullptr);
            OpenGL::CheckErrors();

            glCompileShader(shaderObject);
            OpenGL::CheckErrors();

            // Check compiling results.
            GLint compileStatus = 0;
            glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &compileStatus);
            OpenGL::CheckErrors();

            if(compileStatus == GL_FALSE)
            {
                LOG_ERROR() << "Shader object could not be compiled!";

                GLint errorLength = 0;
                glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &errorLength);
                OpenGL::CheckErrors();

                if(errorLength != 0)
                {
                    std::vector<char> errorText(errorLength);
                    glGetShaderInfoLog(shaderObject, errorLength, &errorLength, &errorText[0]);
                    OpenGL::CheckErrors();

                    LOG_ERROR() << "Shader compile errors: \"" << errorText.data() << "\"";
                }

                return false;
            }
        }
    }

    // Check if any shader objects were found.
    if(shaderObjectsFound == false)
    {
        LOG_ERROR() << "Could not find any shader objects!";
        return false;
    }

    // Create a shader program.
    SCOPE_GUARD_IF(!initialized, this->DestroyHandle());

    m_handle = glCreateProgram();
    OpenGL::CheckErrors();

    if(m_handle == OpenGL::InvalidHandle)
    {
        LOG_ERROR() << "Shader program could not be created!";
        return false;
    }

    // Attach compiled shader objects.
    for(unsigned int i = 0; i < ShaderTypeCount; ++i)
    {
        GLuint& shaderObject = shaderObjects[i];

        if(shaderObject != OpenGL::InvalidHandle)
        {
            glAttachShader(m_handle, shaderObject);
            OpenGL::CheckErrors();
        }
    }

    // Link attached shader objects.
    LOG_INFO() << "Linking shader program...";

    glLinkProgram(m_handle);
    OpenGL::CheckErrors();

    // Detach already linked shader objects.
    for(unsigned int i = 0; i < ShaderTypeCount; ++i)
    {
        GLuint& shaderObject = shaderObjects[i];

        if(shaderObject != OpenGL::InvalidHandle)
        {
            glDetachShader(m_handle, shaderObject);
            OpenGL::CheckErrors();
        }
    }

    // Check linking results.
    GLint linkStatus = 0;
    glGetProgramiv(m_handle, GL_LINK_STATUS, &linkStatus);
    OpenGL::CheckErrors();

    if(linkStatus == GL_FALSE)
    {
        LOG_ERROR()  << "Shader program could not be linked!";

        GLint errorLength = 0;
        glGetProgramiv(m_handle, GL_INFO_LOG_LENGTH, &errorLength);
        OpenGL::CheckErrors();

        if(errorLength != 0)
        {
            std::vector<char> errorText(errorLength);
            glGetProgramInfoLog(m_handle, errorLength, &errorLength, &errorText[0]);
            OpenGL::CheckErrors();

            LOG_ERROR() << "Shader link errors: \"" << errorText.data() << "\"";
        }

        return false;
    }

    // Success!
    return initialized = true;
}

bool Shader::Initialize(const LoadFromFile& params)
{
    LOG() << "Loading shader from \"" << params.filePath << "\" file..." << LOG_INDENT();

    // Validate arguments.
    if(params.engine == nullptr)
    {
        LOG_ERROR() << "Invalid parameter - \"engine\" is null!";
        return false;
    }

    // Resolve the file path.
    std::string resolvedFilePath = params.engine->GetFileSystem().ResolvePath(params.filePath);

    // Load the shader code from a file.
    std::string shaderCode = Utility::GetTextFileContent(resolvedFilePath);

    if(shaderCode.empty())
    {
        LOG_ERROR() << "File could not be read!";
        return false;
    }

    // Call the compile method.
    LoadFromString compileParams;
    compileParams.engine = params.engine;
    compileParams.shaderCode = std::move(shaderCode);

    if(!this->Initialize(compileParams))
    {
        LOG_ERROR() << "Shader code could not be compiled!";
        return false;
    }

    // Success!
    return true;
}

GLint Shader::GetAttributeIndex(std::string name) const
{
    ASSERT(m_handle != OpenGL::InvalidHandle, "Shader program handle has not been created!");
    ASSERT(!name.empty(), "Attribute name cannot be empty!");

    GLint location = glGetAttribLocation(m_handle, name.c_str());

    return location;
}

GLint Shader::GetUniformIndex(std::string name) const
{
    ASSERT(m_handle != OpenGL::InvalidHandle, "Shader program handle has not been created!");
    ASSERT(!name.empty(), "Uniform name cannot be empty!");

    GLint location = glGetUniformLocation(m_handle, name.c_str());

    return location;
}

GLuint Shader::GetHandle() const
{
    ASSERT(m_handle != OpenGL::InvalidHandle, "Shader program handle has not been created!");

    return m_handle;
}

bool Shader::IsValid() const
{
    return m_handle != OpenGL::InvalidHandle;
}
