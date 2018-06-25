/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include <Engine.hpp>
#include <System/Platform.hpp>
#include <System/Window.hpp>
#include <System/Timer.hpp>
#include <Graphics/Buffer.hpp>
#include <Graphics/InputLayout.hpp>
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

    struct Vertex
    {
        glm::vec3 position;
        glm::vec4 color;
    };

    const Vertex vertices[] =
    {
        { glm::vec3( 0.0f,  0.433f,  0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) },
        { glm::vec3( 0.5f, -0.433f,  0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f) },
        { glm::vec3(-0.5f, -0.433f,  0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f) },
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
        { &vertexBuffer, Graphics::InputAttributeTypes::Float4 },
    };

    Graphics::InputLayoutInfo inputLayoutInfo;
    inputLayoutInfo.attributeCount = Utility::StaticArraySize(inputAttributes);
    inputLayoutInfo.attributes = &inputAttributes[0];

    Graphics::InputLayout inputLayout;
    if(!inputLayout.Create(inputLayoutInfo))
        return 1;

    Graphics::Shader shader;
    if(!shader.Load(Build::GetMountDir() + "Data/Shaders/Color.shader"))
        return 1;

    timer.Reset();

    while(window.IsOpen())
    {
        float deltaTime = timer.CalculateFrameDelta();

        window.ProcessEvents();

        editor.Update(deltaTime);

        glClearDepth(1.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader.GetHandle());
        glUniformMatrix4fv(shader.GetUniform("vertexTransform"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

        glBindVertexArray(inputLayout.GetHandle());
        glDrawArrays(GL_TRIANGLES, 0, 3);

        editor.Draw();

        window.Present();

        timer.Tick();
    }

    return 0;
}
