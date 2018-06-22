/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    Graphics Shader
    
    Loads and links GLSL shaders into an OpenGL program object.
    Supports geometry, vertex and fragment shaders.
    
    void ExampleGraphicsShader()
    {
        // Create a shader instance.
        Graphics::Shader shader;
        shader.Load("Data/Shader.glsl");

        // Use the created shader in our rendering pipeline.
        glUseProgram(shader.GetHandle());
        glUniformMatrix4fv(shader.GetUniform("vertexTransform"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
    }

    ExampleShader.glsl
    [
        #version 330
        
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
    // Shader class.
    class Shader
    {
    public:
        Shader();
        ~Shader();

        // Loads the shader from a file.
        bool Load(std::string filePath);

        // Compiles the shader from code.
        bool Compile(std::string shaderCode);

        // Gets an attribute index from the shader program.
        GLint GetAttribute(std::string name) const;

        // Gets an uniform index from the shader program.
        GLint GetUniform(std::string name) const;

        // Gets the shader's program handle.
        GLuint GetHandle() const;

        // Checks if the shader is valid.
        bool IsValid() const;

    private:
        // Destroys the internal handle.
        void DestroyHandle();

    private:
        // Program handle.
        GLuint m_handle;
    };
}
