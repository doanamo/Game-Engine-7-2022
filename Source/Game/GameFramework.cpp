/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Game/GameFramework.hpp"
#include "Game/GameInstance.hpp"
#include <System/Window.hpp>
#include <Renderer/GameRenderer.hpp>
using namespace Game;

GameFramework::GameFramework() = default;
GameFramework::~GameFramework() = default;

GameFramework::CreateResult GameFramework::Create(const CreateFromParams& params)
{
    LOG("Creating game framework...");
    LOG_SCOPED_INDENT();

    // Check arguments.
    CHECK_ARGUMENT_OR_RETURN(params.services != nullptr, Common::Failure(CreateErrors::InvalidArgument));

    // Create instance.
    auto instance = std::unique_ptr<GameFramework>(new GameFramework());

    // Save system references.
    instance->m_timer = params.services->GetTimer();
    instance->m_window = params.services->GetWindow();
    instance->m_gameRenderer = params.services->GetGameRenderer();

    // Success!
    return Common::Success(std::move(instance));
}

bool GameFramework::Update()
{
    // Tick game instance and return true if update occurred.
    if(m_gameInstance)
    {
        return m_gameInstance->Tick(*m_timer);
    }

    // Return false if game instance did not update.
    return false;
}

void GameFramework::Draw()
{
    // Get window viewport rect.
    glm::ivec4 viewportRect = { 0, 0, m_window->GetWidth(), m_window->GetHeight() };

    // Draw game instance.
    Renderer::GameRenderer::DrawParams drawParams;
    drawParams.viewportRect = viewportRect;
    drawParams.gameInstance = m_gameInstance.get();
    drawParams.cameraName = "Camera";
    m_gameRenderer->Draw(drawParams);
}

void GameFramework::SetGameInstance(std::shared_ptr<GameInstance> gameInstance)
{
    // Make sure we are not setting the same game instance.
    if(gameInstance == m_gameInstance)
    {
        LOG_WARNING("Attempted to change game instance into the current one!");
        return;
    }

    // Change the current game instance.
    m_gameInstance = gameInstance;

    // Notify listeners about game instance being changed.
    events.gameInstanceChanged.Dispatch(m_gameInstance);
}

std::shared_ptr<GameInstance> GameFramework::GetGameInstance() const
{
    return m_gameInstance;
}
