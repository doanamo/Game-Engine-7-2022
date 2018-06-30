/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include <Engine.hpp>
#include <System/Platform.hpp>
#include <System/Window.hpp>
#include <System/Timer.hpp>
#include <Graphics/ScreenSpace.hpp>
#include <Graphics/Buffer.hpp>
#include <Graphics/InputLayout.hpp>
#include <Graphics/Texture.hpp>
#include <Graphics/Sampler.hpp>
#include <Graphics/Shader.hpp>
#include <Editor/Editor.hpp>

int main()
{
    Build::Initialize();
    Debug::Initialize();
    Logger::Initialize();

    {
        LOG() << "Build info:" << LOG_INDENT();
        LOG() << "Build directory: " << Build::GetBuildDir();
        LOG() << "Include directory: " << Build::GetIncludeDir();
        LOG() << "Source directory: " << Build::GetSourceDir();
        LOG() << "Change number: " << Build::GetChangeNumber();
        LOG() << "Change hash: " << Build::GetChangeHash();
        LOG() << "Change date: " << Build::GetChangeDate();
        LOG() << "Branch name: " << Build::GetBranchName();
    }

    System::Platform platform;
    if(!platform.Initialize())
        return 1;

    System::WindowInfo windowInfo;
    windowInfo.title = "Engine Example";
    windowInfo.width = 1024;
    windowInfo.height = 576;
    windowInfo.vsync = false;
    windowInfo.visible = true;

    System::Window window;
    if(!window.Open(windowInfo))
        return 1;

    System::Timer timer;

    Engine::Editor editor;
    if(!editor.Initialize(&window))
        return 1;

    Graphics::ScreenSpace screenSpace;
    screenSpace.SetSourceSize(2.0f, 2.0f);

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

    Graphics::VertexBuffer vertexBuffer;
    if(!vertexBuffer.Create(bufferInfo))
        return 1;

    const Graphics::InputAttribute inputAttributes[] =
    {
        { &vertexBuffer, Graphics::InputAttributeTypes::Float3 },
        { &vertexBuffer, Graphics::InputAttributeTypes::Float2 },
        { &vertexBuffer, Graphics::InputAttributeTypes::Float4 },
    };

    Graphics::InputLayoutInfo inputLayoutInfo;
    inputLayoutInfo.attributeCount = Utility::StaticArraySize(inputAttributes);
    inputLayoutInfo.attributes = &inputAttributes[0];

    Graphics::InputLayout inputLayout;
    if(!inputLayout.Create(inputLayoutInfo))
        return 1;

    Graphics::Texture texture;
    if(!texture.Load(Build::GetMountDir() + "Data/Textures/Checker.png"))
        return 1;

    Graphics::Sampler sampler;
    if(!sampler.Create())
        return -1;

    Graphics::Shader shader;
    if(!shader.Load(Build::GetMountDir() + "Data/Shaders/Textured.shader"))
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

        glBindVertexArray(inputLayout.GetHandle());
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        editor.Draw();

        window.Present();

        timer.Tick();
    }

    return 0;
}
