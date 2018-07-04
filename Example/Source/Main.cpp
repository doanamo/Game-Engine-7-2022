/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include <Engine.hpp>
#include <System/Platform.hpp>
#include <System/Window.hpp>
#include <System/Timer.hpp>
#include <Graphics/RenderContext.hpp>
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

    // Initialize the platform context.
    System::Platform platform;
    if(!platform.Initialize())
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

    // Create the graphics context.
    Graphics::RenderContext renderContext;
    if(!renderContext.Initialize(&window))
        return 1;

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

    Graphics::VertexBuffer vertexBuffer(&renderContext);
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

    Graphics::VertexArray vertexArray(&renderContext);
    if(!vertexArray.Create(inputLayoutInfo))
        return 1;

    // Load a texture.
    Graphics::Texture texture(&renderContext);
    if(!texture.Load(Build::GetWorkingDir() + "Data/Textures/Checker.png"))
        return 1;

    // Create a sampler.
    Graphics::Sampler sampler(&renderContext);
    if(!sampler.Create())
        return 1;

    // Load a shader.
    Graphics::Shader shader(&renderContext);
    if(!shader.Load(Build::GetWorkingDir() + "Data/Shaders/Textured.shader"))
        return 1;

    // Create a screen space.
    Graphics::ScreenSpace screenSpace;
    screenSpace.SetSourceSize(2.0f, 2.0f);

    // Initialize the editor.
    Engine::Editor editor(&renderContext);
    if(!editor.Initialize(&window))
        return 1;

    // Create a timer.
    System::Timer timer;

    // Main processing loop.
    while(window.IsOpen())
    {
        // Calculate frame delta time.
        float deltaTime = timer.CalculateFrameDelta();

        // Process window events.
        window.ProcessEvents();

        // Update the editor interface.
        editor.Update(deltaTime);

        // Get the render state.
        Graphics::RenderState& renderState = renderContext.GetState();

        // Create a viewport.
        renderState.Viewport(0, 0, window.GetWidth(), window.GetHeight());
        screenSpace.SetTargetSize(window.GetWidth(), window.GetHeight());

        // Calculate combined view and projection matrix.
        glm::mat4 transform = screenSpace.GetTransform();
        transform = glm::translate(transform, glm::vec3(-screenSpace.GetOffsetFromCenter(), 0.0f));

        // Render a rectangle.
        renderState.ClearDepth(1.0f);
        renderState.ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        renderState.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderState.ActiveTexture(GL_TEXTURE0);
        renderState.BindTexture(GL_TEXTURE_2D, texture.GetHandle());
        renderState.BindSampler(0, sampler.GetHandle());

        renderState.UseProgram(shader.GetHandle());
        shader.SetUniform("vertexTransform", transform);
        shader.SetUniform("textureDiffuse", 0);

        renderState.BindVertexArray(vertexArray.GetHandle());
        renderState.DrawArrays(GL_TRIANGLE_FAN, 0, 4);

        // Draw the editor interface.
        editor.Draw();

        // Present window content.
        window.Present();

        // Tick the timer.
        timer.Tick();
    }

    return 0;
}
