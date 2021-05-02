/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Core/ServiceStorage.hpp>
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

    Draws editor interface.
*/

namespace Editor
{
    class EditorRenderer final : private Common::NonCopyable
    {
    public:
        struct CreateFromParams
        {
            const Core::ServiceStorage* services = nullptr;
        };

        enum class CreateErrors
        {
            InvalidArgument,
            FailedResourceCreation,
        };

        using CreateResult = Common::Result<std::unique_ptr<EditorRenderer>, CreateErrors>;
        static CreateResult Create(const CreateFromParams& params);

        ~EditorRenderer();

        void Draw();

    private:
        EditorRenderer();

        bool CreateResources(const Core::ServiceStorage* serviceStorage);

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
