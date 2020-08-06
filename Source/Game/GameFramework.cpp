/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Game/GameFramework.hpp"
#include "Game/GameState.hpp"
#include <System/Window.hpp>
#include <Renderer/StateRenderer.hpp>
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

    // Create event router.
    // Listens and replicates event to the current game state.
    EventRouter::CreateFromParams eventRouterParams;
    eventRouterParams.services = params.services;
    eventRouterParams.gameFramework = instance.get();

    instance->m_eventRouter = EventRouter::Create(eventRouterParams).UnwrapOr(nullptr);
    if(instance->m_eventRouter == nullptr)
    {
        LOG_ERROR("Could not create event router!");
        return Common::Failure(CreateErrors::FailedEventRouterCreation);
    }

    // Save system references.
    instance->m_timer = params.services->GetTimer();
    instance->m_window = params.services->GetWindow();
    instance->m_stateRenderer = params.services->GetStateRenderer();

    // Success!
    return Common::Success(std::move(instance));
}

bool GameFramework::Update()
{
    // Update game state.
    if(m_gameState)
    {
        // Update and return true if update occurred.
        return m_gameState->Update(*m_timer);
    }

    // Return false if game state did not update.
    return false;
}

void GameFramework::Draw()
{
    // Get window viewport rect.
    glm::ivec4 viewportRect = { 0, 0, m_window->GetWidth(), m_window->GetHeight() };

    // Draw game state.
    Renderer::StateRenderer::DrawParams drawParams;
    drawParams.viewportRect = viewportRect;
    drawParams.gameState = m_gameState.get();
    drawParams.cameraName = "Camera";
    m_stateRenderer->Draw(drawParams);
}

void GameFramework::SetGameState(std::shared_ptr<GameState> gameState)
{
    // Make sure we are not setting the same game state.
    if(gameState == m_gameState)
    {
        LOG_WARNING("Attempted to change game state into the current one!");
        return;
    }

    // Notify current game state about being changed.
    if(m_gameState)
    {
        Game::GameState::Events::GameStateChanged gameStateChanged;
        gameStateChanged.stateEntered = false;

        m_gameState->eventQueue.Push(gameStateChanged);
    }

    // Change the current game state.
    m_gameState = gameState;

    // Notify new game state about being changed.
    if(gameState)
    {
        Game::GameState::Events::GameStateChanged gameStateChanged;
        gameStateChanged.stateEntered = true;

        gameState->eventQueue.Push(gameStateChanged);
    }

    // Notify listeners about game state being changed.
    events.gameStateChanged.Dispatch(m_gameState);
}

std::shared_ptr<GameState> GameFramework::GetGameState() const
{
    return m_gameState;
}
