/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Editor/TextureViewer.hpp"
#include "Engine.hpp"
using namespace Editor;

TextureViewer::TextureViewer() :
    m_engine(nullptr),
    m_initialized(false)
{
}

TextureViewer::~TextureViewer()
{
}

TextureViewer::TextureViewer(TextureViewer&& other) :
    TextureViewer()
{
    // Call the move assignment.
    *this = std::move(other);
}

TextureViewer& TextureViewer::operator=(TextureViewer&& other)
{
    // Swap class members.
    std::swap(m_engine, other.m_engine);
    std::swap(m_screenSpace, other.m_screenSpace);
    std::swap(m_texture, other.m_texture);
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool TextureViewer::Initialize(Engine::Root* engine)
{
    LOG() << "Initializing texture viewer..." << LOG_INDENT();

    // Make sure instance is not initialized.
    VERIFY(!m_initialized, "Texture viewer instance has already been initialized!");

    // Reset class instance if initialization fails.
    SCOPE_GUARD_IF(!m_initialized, *this = TextureViewer());

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
    Graphics::Texture::LoadFromFile textureParams;
    textureParams.engine = m_engine;
    textureParams.filePath = "Data/Engine/Textures/Checker.png";

    m_texture = m_engine->resourceManager.Acquire<Graphics::Texture>(
        textureParams.filePath, textureParams);

    // Success!
    return m_initialized = true;
}

void TextureViewer::OnUpdate(float timeDelta)
{
    ASSERT(m_initialized, "Texture viever instance has not been initialized!");

    // Define editor interface.

}

void TextureViewer::OnDraw(const Game::SceneDrawParams& drawParams)
{
    ASSERT(m_initialized, "Texture viever instance has not been initialized!");

    // Push a new render state.
    auto& renderState = m_engine->renderContext.GetState();

    // Define screen space size.
    glm::ivec2 viewportSize = drawParams.CalculateViewportSize();
    m_screenSpace.SetTargetSize(viewportSize.x, viewportSize.y);

    // Calculate combined view and projection matrix.
    glm::mat4 vertexTransform = m_screenSpace.GetTransform();
    vertexTransform = glm::translate(vertexTransform, glm::vec3(-m_screenSpace.GetOffsetFromCenter(), 0.0f));

    // Draw the viewed texture.
    if(m_texture)
    {   
        Graphics::Sprite sprite;
        sprite.info.texture = m_texture.get();
        sprite.info.transparent = false;
        sprite.info.filtered = true;
        sprite.data.transform = glm::mat4(1.0f);
        sprite.data.rectangle = glm::vec4(-0.5f, -0.5f, 0.5f, 0.5f);
        sprite.data.coords = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
        sprite.data.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

        Graphics::SpriteDrawList spriteDrawList;
        spriteDrawList.AddSprite(sprite);

        m_engine->spriteRenderer.DrawSprites(spriteDrawList, vertexTransform);
    }
}
