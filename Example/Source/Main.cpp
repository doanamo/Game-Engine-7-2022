/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include <Engine.hpp>
#include <System/Context.hpp>
#include <System/Window.hpp>
#include <System/Timer.hpp>
#include <Graphics/Context.hpp>
#include <Graphics/ScreenSpace.hpp>
#include <Graphics/Buffer.hpp>
#include <Graphics/VertexArray.hpp>
#include <Graphics/Texture.hpp>
#include <Graphics/Sampler.hpp>
#include <Graphics/Shader.hpp>
#include <Editor/Editor.hpp>

int main()
{
    // Initialize low level systems.
    Build::Initialize();
    Debug::Initialize();
    Logger::Initialize();

    // Print build info.
    {
        LOG_INFO() << "Build info:" << LOG_INDENT();
        LOG_INFO() << "Build directory: " << Build::GetBuildDir();
        LOG_INFO() << "Working directory: " << Build::GetWorkingDir();
        LOG_INFO() << "Include directory: " << Build::GetIncludeDir();
        LOG_INFO() << "Source directory: " << Build::GetSourceDir();
        LOG_INFO() << "Change number: " << Build::GetChangeNumber();
        LOG_INFO() << "Change hash: " << Build::GetChangeHash();
        LOG_INFO() << "Change date: " << Build::GetChangeDate();
        LOG_INFO() << "Branch name: " << Build::GetBranchName();
    }

    // Initialize the system context.
    System::Context system;
    if(!system.Initialize())
        return 1;

    // Open a window.
    System::WindowInfo windowInfo;
    windowInfo.title = "Engine Example";
    windowInfo.width = 1024;
    windowInfo.height = 576;
    windowInfo.vsync = false;
    windowInfo.visible = true;

    System::Window window;
    if(!window.Open(windowInfo))
        return 1;

    // Create a timer.
    System::Timer timer;

    // Create the graphics context.
    Graphics::Context graphics;
    if(!graphics.Initialize(&window))
        return -1;

    // Create the rendering screen space.
    Graphics::ScreenSpace screenSpace;
    screenSpace.SetSourceSize(2.0f, 2.0f);

    // Create a vertex buffer.
    struct Vertex
    {
        glm::vec3 position;
        glm::vec2 texture;
        glm::vec4 color;
    };

    const Vertex vertices[] =
    {
        { glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) },
        { glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) },
        { glm::vec3( 0.5f,  0.5f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) },
        { glm::vec3( 0.5f, -0.5f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) },
    };

    Graphics::BufferInfo bufferInfo;
    bufferInfo.elementSize = sizeof(Vertex);
    bufferInfo.elementCount = Utility::StaticArraySize(vertices);
    bufferInfo.data = &vertices[0];

    Graphics::VertexBuffer vertexBuffer(&graphics);
    if(!vertexBuffer.Create(bufferInfo))
        return 1;

    // Create a vertex array.
    const Graphics::VertexAttribute inputAttributes[] =
    {
        { &vertexBuffer, Graphics::VertexAttributeType::Vector3, GL_FLOAT, false },
        { &vertexBuffer, Graphics::VertexAttributeType::Vector2, GL_FLOAT, false },
        { &vertexBuffer, Graphics::VertexAttributeType::Vector4, GL_FLOAT, false },
    };

    Graphics::VertexArrayInfo inputLayoutInfo;
    inputLayoutInfo.attributeCount = Utility::StaticArraySize(inputAttributes);
    inputLayoutInfo.attributes = &inputAttributes[0];

    Graphics::VertexArray vertexArray(&graphics);
    if(!vertexArray.Create(inputLayoutInfo))
        return 1;

    Graphics::Texture texture;
    if(!texture.Load(Build::GetWorkingDir() + "Data/Textures/Checker.png"))
        return 1;

    Graphics::Sampler sampler;
    if(!sampler.Create())
        return -1;

    Graphics::Shader shader;
    if(!shader.Load(Build::GetWorkingDir() + "Data/Shaders/Textured.shader"))
        return 1;

    Engine::Editor editor(&graphics);
    if(!editor.Initialize(&window))
        return 1;

    timer.Reset();

    while(window.IsOpen())
    {
        float deltaTime = timer.CalculateFrameDelta();

        window.ProcessEvents();

        editor.Update(deltaTime);

        glViewport(0, 0, window.GetWidth(), window.GetHeight());
        screenSpace.SetTargetSize(window.GetWidth(), window.GetHeight());

        glClearDepth(1.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 transform = screenSpace.GetTransform();
        transform = glm::translate(transform, glm::vec3(-screenSpace.GetOffsetFromCenter(), 0.0f));

        glUseProgram(shader.GetHandle());
        glUniformMatrix4fv(shader.GetUniform("vertexTransform"), 1, GL_FALSE, glm::value_ptr(transform));
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture.GetHandle());
        glBindSampler(0, sampler.GetHandle());
        glUniform1i(shader.GetUniform("textureDiffuse"), 0);

        glBindVertexArray(vertexArray.GetHandle());
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        editor.Draw();

        window.Present();

        timer.Tick();
    }

    return 0;
}
