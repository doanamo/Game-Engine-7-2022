/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Editor/TextureEditor.hpp"
#include "Engine.hpp"
using namespace Editor;

TextureEditor::TextureEditor() :
    m_engine(nullptr),
    m_initialized(false)
{
}

TextureEditor::~TextureEditor()
{
}

TextureEditor::TextureEditor(TextureEditor&& other) :
    TextureEditor()
{
    // Call the move assignment.
    *this = std::move(other);
}

TextureEditor& TextureEditor::operator=(TextureEditor&& other)
{
    // Swap class members.
    std::swap(m_engine, other.m_engine);
    std::swap(m_screenSpace, other.m_screenSpace);
    std::swap(m_texture, other.m_texture);
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool TextureEditor::Initialize(Engine::Root* engine)
{
    LOG() << "Initializing texture viewer...";

    // Make sure instance is not initialized.
    VERIFY(!m_initialized, "Texture viewer instance has already been initialized!");

    // Reset class instance if initialization fails.
    SCOPE_GUARD_IF(!m_initialized, *this = TextureEditor());

    // Validate and save engine reference.
    if(engine == nullptr || !engine->IsInitialized())
    {
        LOG_ERROR() << "Invalid argument - \"engine\" is invalid!";
        return false;
    }

    m_engine = engine;

    // Define screen space size.
    m_screenSpace.SetSourceSize(2.0f, 2.0f);

    // Load default texture.
    Graphics::TextureLoadInfo textureInfo;
    textureInfo.filePath = Build::GetEngineDir() + "Data/Engine/Textures/Checker.png";

    m_texture = m_engine->resourceManager.Acquire<Graphics::Texture>(
        textureInfo.filePath, &m_engine->renderContext, textureInfo);

    // Success!
    return m_initialized = true;
}

void TextureEditor::OnUpdate(float timeDelta)
{
    ASSERT(m_initialized, "Texture viever instance has not been initialized!");

    // Define editor interface.

}

void TextureEditor::OnDraw(const Game::SceneDrawParams& drawParams)
{
    ASSERT(m_initialized, "Texture viever instance has not been initialized!");

    // Push a new render state.
    auto& renderState = m_engine->renderContext.GetState();

    // Define screen space size.
    glm::ivec2 viewportSize = drawParams.GetViewportSize();
    m_screenSpace.SetTargetSize(viewportSize.x, viewportSize.y);

    // Calculate combined view and projection matrix.
    glm::mat4 vertexTransform = m_screenSpace.GetTransform();
    vertexTransform = glm::translate(vertexTransform, glm::vec3(-m_screenSpace.GetOffsetFromCenter(), 0.0f));

    // Draw the viewed texture.
    if(m_texture)
    {   
        Graphics::Sprite sprite;
        sprite.info.transparent = false;
        sprite.info.texture = m_texture.get();
        sprite.info.filter = true;
        sprite.data.transform = glm::mat4(1.0f);
        sprite.data.spriteRectangle = glm::vec4(-0.5f, -0.5f, 0.5f, 0.5f);
        sprite.data.textureRectangle = glm::vec4(0.0f, 0.0f, m_texture->GetWidth(), m_texture->GetHeight());
        sprite.data.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

        Graphics::SpriteList spriteList;
        spriteList.AddSprite(sprite);

        m_engine->spriteRenderer.DrawSprites(spriteList, vertexTransform);
    }
}
