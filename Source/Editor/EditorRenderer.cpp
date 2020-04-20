/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Editor/EditorRenderer.hpp"
#include <System/Window.hpp>
#include <System/ResourceManager.hpp>
using namespace Editor;

EditorRenderer::EditorRenderer() = default;
EditorRenderer::~EditorRenderer() = default;

EditorRenderer::CreateResult EditorRenderer::Create(const CreateFromParams& params)
{
    LOG("Creating editor renderer...");
    LOG_SCOPED_INDENT();

    // Validate references.
    CHECK_ARGUMENT_OR_RETURN(params.window != nullptr, Common::Failure(CreateErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(params.fileSystem != nullptr, Common::Failure(CreateErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(params.resourceManager != nullptr, Common::Failure(CreateErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(params.renderContext != nullptr, Common::Failure(CreateErrors::InvalidArgument));

    // Create instance.
    auto instance = std::unique_ptr<EditorRenderer>(new EditorRenderer());

    // We are expecting ImGui context to be set by the caller.
    ASSERT(ImGui::GetCurrentContext() != nullptr, "Editor interface context is not set!");

    // Create vertex buffer.
    Graphics::Buffer::CreateFromParams vertexBufferParams;
    vertexBufferParams.usage = GL_STREAM_DRAW;
    vertexBufferParams.elementSize = sizeof(ImDrawVert);

    instance->m_vertexBuffer = Graphics::VertexBuffer::Create(params.renderContext, vertexBufferParams).UnwrapOr(nullptr);
    if(instance->m_vertexBuffer == nullptr)
    {
        LOG_ERROR("Could not create vertex buffer!");
        return Common::Failure(CreateErrors::FailedResourceCreation);
    }

    // Create index buffer.
    Graphics::Buffer::CreateFromParams indexBufferParams;
    indexBufferParams.usage = GL_STREAM_DRAW;
    indexBufferParams.elementSize = sizeof(ImDrawIdx);

    instance->m_indexBuffer = Graphics::IndexBuffer::Create(params.renderContext, indexBufferParams).UnwrapOr(nullptr);
    if(instance->m_indexBuffer == nullptr)
    {
        LOG_ERROR("Could not create index buffer!");
        return Common::Failure(CreateErrors::FailedResourceCreation);
    }

    // Create input layout.
    const Graphics::VertexArray::Attribute inputAttributes[] =
    {
        { instance->m_vertexBuffer.get(), Graphics::VertexArray::AttributeType::Vector2, GL_FLOAT,         false }, // Position
        { instance->m_vertexBuffer.get(), Graphics::VertexArray::AttributeType::Vector2, GL_FLOAT,         false }, // Texture
        { instance->m_vertexBuffer.get(), Graphics::VertexArray::AttributeType::Vector4, GL_UNSIGNED_BYTE, true }, // Color
    };

    Graphics::VertexArray::FromArrayParams inputLayoutParams;
    inputLayoutParams.attributeCount = Common::StaticArraySize(inputAttributes);
    inputLayoutParams.attributes = &inputAttributes[0];

    instance->m_vertexArray = Graphics::VertexArray::Create(params.renderContext, inputLayoutParams).UnwrapOr(nullptr);
    if(instance->m_vertexArray == nullptr)
    {
        LOG_ERROR("Could not create vertex array!");
        return Common::Failure(CreateErrors::FailedResourceCreation);
    }

    // Retrieve built in font data.
    unsigned char* fontData = nullptr;

    int fontWidth = 0;
    int fontHeight = 0;

    ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&fontData, &fontWidth, &fontHeight);

    if(fontData == nullptr || fontWidth == 0 || fontHeight == 0)
    {
        LOG_ERROR("Could not retrieve font data!");
        return Common::Failure(CreateErrors::FailedResourceCreation);
    }

    // Create font texture.
    Graphics::Texture::CreateFromParams textureParams;
    textureParams.renderContext = params.renderContext;
    textureParams.width = fontWidth;
    textureParams.height = fontHeight;
    textureParams.format = GL_RGBA;
    textureParams.mipmaps = false;
    textureParams.data = fontData;

    instance->m_fontTexture = Graphics::Texture::Create(textureParams).UnwrapOr(nullptr);
    if(instance->m_fontTexture == nullptr)
    {
        LOG_ERROR("Could not create font texture!");
        return Common::Failure(CreateErrors::FailedResourceCreation);
    }

    ImGui::GetIO().Fonts->TexID = (void*)(intptr_t)instance->m_fontTexture->GetHandle();

    // Create sampler.
    // Set linear filtering otherwise textures without mipmaps will be black.
    Graphics::Sampler::CreateFromParams samplerParams;
    samplerParams.textureMinFilter = GL_LINEAR;
    samplerParams.textureMagFilter = GL_LINEAR;

    instance->m_sampler = Graphics::Sampler::Create(params.renderContext, samplerParams).UnwrapOr(nullptr);
    if(instance->m_sampler == nullptr)
    {
        LOG_ERROR("Could not create sampler!");
        return Common::Failure(CreateErrors::FailedResourceCreation);
    }

    // Load shader.
    Graphics::Shader::LoadFromFile shaderParams;
    shaderParams.fileSystem = params.fileSystem;
    shaderParams.filePath = "Data/Engine/Shaders/Interface.shader";
    shaderParams.renderContext = params.renderContext;

    instance->m_shader = params.resourceManager->Acquire<Graphics::Shader>(
        shaderParams.filePath, shaderParams).UnwrapOr(nullptr);

    if(instance->m_shader == nullptr)
    {
        LOG_ERROR("Could not create shader!");
        return Common::Failure(CreateErrors::FailedResourceCreation);
    }

    // Save system references.
    instance->m_window = params.window;
    instance->m_renderContext = params.renderContext;

    // Success!
    return Common::Success(std::move(instance));
}

void EditorRenderer::Draw()
{
    // We are expecting ImGui context to be set by the caller.
    ASSERT(ImGui::GetCurrentContext() != nullptr, "Editor interface context is not set!");

    // Get current window dimensions.
    int windowWidth = m_window->GetWidth();
    int windowHeight = m_window->GetHeight();

    // Access ImGui context.
    ImGuiIO& io = ImGui::GetIO();

    // Set current display size.
    io.DisplaySize.x = (float)windowWidth;
    io.DisplaySize.y = (float)windowHeight;

    // Handle case where Update() has not been called yet.
    // Calling NewFrame() twice in a row should be fine.
    if(ImGui::GetFrameCount() == 0)
    {
        ImGui::NewFrame();
    }

    // End our rendering frame.
    ImGui::EndFrame();
    ImGui::Render();

    // Get interface render data.
    ImDrawData* drawData = ImGui::GetDrawData();

    // Calculate rendering transform.
    glm::mat4 transform = glm::ortho(0.0f, (float)windowWidth, (float)windowHeight, 0.0f);

    // Push rendering state.
    auto& renderState = m_renderContext->PushState();
    SCOPE_GUARD(m_renderContext->PopState());

    // Prepare rendering state.
    renderState.Enable(GL_BLEND);
    renderState.BlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    renderState.BlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    renderState.Enable(GL_SCISSOR_TEST);

    renderState.Viewport(0, 0, windowWidth, windowHeight);

    renderState.UseProgram(m_shader->GetHandle());
    m_shader->SetUniform("vertexTransform", transform);
    m_shader->SetUniform("textureDiffuse", 0);

    renderState.BindSampler(0, m_sampler->GetHandle());

    // Process draw data.
    ImVec2 position = drawData->DisplayPos;
    for(int list = 0; list < drawData->CmdListsCount; ++list)
    {
        const ImDrawList* commandList = drawData->CmdLists[list];
        const ImDrawIdx* indexBufferOffset = 0;

        m_vertexBuffer->Update(commandList->VtxBuffer.Data, commandList->VtxBuffer.Size);
        m_indexBuffer->Update(commandList->IdxBuffer.Data, commandList->IdxBuffer.Size);

        for(int command = 0; command < commandList->CmdBuffer.Size; ++command)
        {
            const ImDrawCmd* drawCommand = &commandList->CmdBuffer[command];

            if(drawCommand->UserCallback)
            {
                drawCommand->UserCallback(commandList, drawCommand);
            }
            else
            {
                ImVec4 clipRect;
                clipRect.x = drawCommand->ClipRect.x - position.x;
                clipRect.y = drawCommand->ClipRect.y - position.y;
                clipRect.z = drawCommand->ClipRect.z - position.x;
                clipRect.w = drawCommand->ClipRect.w - position.y;

                glm::ivec4 scissorRect;
                scissorRect.x = (int)clipRect.x;
                scissorRect.y = (int)(windowHeight - clipRect.w);
                scissorRect.z = (int)(clipRect.z - clipRect.x);
                scissorRect.w = (int)(clipRect.w - clipRect.y);

                renderState.Scissor(scissorRect.x, scissorRect.y, scissorRect.z, scissorRect.w);

                renderState.ActiveTexture(GL_TEXTURE0);
                renderState.BindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)drawCommand->TextureId);

                renderState.BindVertexArray(m_vertexArray->GetHandle());
                renderState.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer->GetHandle());
                renderState.DrawElements(GL_TRIANGLES, (GLsizei)drawCommand->ElemCount, GL_UNSIGNED_SHORT, indexBufferOffset);
            }

            indexBufferOffset += drawCommand->ElemCount;
        }
    }
}
