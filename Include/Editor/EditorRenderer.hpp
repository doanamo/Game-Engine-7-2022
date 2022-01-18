/*
    Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Core/System/EngineSystem.hpp>
#include <Graphics/RenderContext.hpp>
#include <Graphics/Buffer.hpp>
#include <Graphics/VertexArray.hpp>
#include <Graphics/Texture.hpp>
#include <Graphics/Sampler.hpp>
#include <Graphics/Shader.hpp>
#include "Editor/EditorSubsystem.hpp"

namespace Platform
{
    class WindowSystem;
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
    class EditorRenderer final : private EditorSubsystem
    {
        REFLECTION_ENABLE(EditorRenderer, EditorSubsystem)

    public:
        EditorRenderer();
        ~EditorRenderer();

    private:
        bool OnAttach(const EditorSubsystemStorage& editorSubsystems) override;
        bool CreateResources(const Core::EngineSystemStorage& engineSystems);
        void OnEndInterface() override;

    private:
        Platform::WindowSystem* m_windowSystem = nullptr;
        Graphics::RenderContext* m_renderContext = nullptr;

        std::unique_ptr<Graphics::VertexBuffer> m_vertexBuffer;
        std::unique_ptr<Graphics::IndexBuffer> m_indexBuffer;
        std::unique_ptr<Graphics::VertexArray> m_vertexArray;
        std::unique_ptr<Graphics::Texture> m_fontTexture;
        std::unique_ptr<Graphics::Sampler> m_sampler;
        std::shared_ptr<Graphics::Shader> m_shader;
    };
}

REFLECTION_TYPE(Editor::EditorRenderer, Editor::EditorSubsystem)
