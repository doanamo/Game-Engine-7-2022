/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <Graphics/RenderContext.hpp>
#include <Graphics/Buffer.hpp>
#include <Graphics/VertexArray.hpp>
#include <Graphics/Texture.hpp>
#include <Graphics/Sampler.hpp>
#include <Graphics/Shader.hpp>

namespace System
{
    class Window;
    class FileSystem;
    class ResourceManager;
}

namespace Graphics
{
    class RenderContext;
}

/*
    Editor Renderer
*/

namespace Editor
{
    class EditorRenderer final : private NonCopyable, public Resettable<EditorRenderer>
    {
    public:
        enum class InitializeErrors
        {
            InvalidArgument,
            FailedResourceCreation,
        };

        using InitializeResult = Result<void, InitializeErrors>;

        struct InitializeFromParams
        {
            System::Window* window = nullptr;
            System::FileSystem* fileSystem = nullptr;
            System::ResourceManager* resourceManager = nullptr;
            Graphics::RenderContext* renderContext = nullptr;
        };

    public:
        EditorRenderer();
        ~EditorRenderer();

        InitializeResult Initialize(const InitializeFromParams& params);
        void Draw();

    private:
        System::Window* m_window = nullptr;
        Graphics::RenderContext* m_renderContext = nullptr;

        Graphics::VertexBuffer m_vertexBuffer;
        Graphics::IndexBuffer m_indexBuffer;
        Graphics::VertexArray m_vertexArray;
        Graphics::Texture m_fontTexture;
        Graphics::Sampler m_sampler;
        Graphics::ShaderPtr m_shader;

        bool m_initialized = false;
    };
}
