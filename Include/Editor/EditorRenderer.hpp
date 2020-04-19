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
    class EditorRenderer final : private NonCopyable
    {
    public:
        struct CreateFromParams
        {
            System::Window* window = nullptr;
            System::FileSystem* fileSystem = nullptr;
            System::ResourceManager* resourceManager = nullptr;
            Graphics::RenderContext* renderContext = nullptr;
        };

        enum class CreateErrors
        {
            InvalidArgument,
            FailedResourceCreation,
        };

        using CreateResult = Result<std::unique_ptr<EditorRenderer>, CreateErrors>;
        static CreateResult Create(const CreateFromParams& params);

    public:
        ~EditorRenderer();

        void Draw();

    private:
        EditorRenderer();

    private:
        System::Window* m_window = nullptr;
        Graphics::RenderContext* m_renderContext = nullptr;

        std::unique_ptr<Graphics::VertexBuffer> m_vertexBuffer;
        std::unique_ptr<Graphics::IndexBuffer> m_indexBuffer;
        std::unique_ptr<Graphics::VertexArray> m_vertexArray;
        std::unique_ptr<Graphics::Texture> m_fontTexture;
        std::unique_ptr<Graphics::Sampler> m_sampler;
        std::shared_ptr<Graphics::Shader> m_shader;
    };
}
