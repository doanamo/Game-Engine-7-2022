/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include <Engine.hpp>
#include <System/Platform.hpp>
#include <System/Window.hpp>
#include <Graphics/Buffer.hpp>
#include <Graphics/InputLayout.hpp>

int main()
{
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

    glBindVertexArray(inputLayout.GetHandle());

    while(window.IsOpen())
    {
        window.ProcessEvents();

        window.Present();
    }

    return 0;
}
