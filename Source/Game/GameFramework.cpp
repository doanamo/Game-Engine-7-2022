/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Game/GameFramework.hpp"
#include "Game/GameState.hpp"
#include <System/Window.hpp>
#include <Renderer/StateRenderer.hpp>
using namespace Game;

GameFramework::GameFramework() = default;
GameFramework::~GameFramework() = default;

GameFramework::InitializeResult GameFramework::Initialize(const InitializeFromParams& params)
{
    LOG("Initializing game framework...");
    LOG_SCOPED_INDENT();

    // Setup initialization guard.
    VERIFY(!m_initialized, "Instance has already been initialized!");
    SCOPE_GUARD_IF(!m_initialized, this->Reset());

    // Check arguments.
    CHECK_ARGUMENT_OR_RETURN(params.timer != nullptr, Failure(InitializeErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(params.window != nullptr, Failure(InitializeErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(params.stateRenderer != nullptr, Failure(InitializeErrors::InvalidArgument));

    m_timer = params.timer;
    m_window = params.window;
    m_stateRenderer = params.stateRenderer;

    // Initialize the event router.
    // Listens and replicates event to the current game state.
    EventRouter::InitializeFromParams eventRouterParams;
    eventRouterParams.inputManager = params.inputManager;
    eventRouterParams.gameFramework = this;

    if(!m_eventRouter.Initialize(eventRouterParams))
    {
        LOG_ERROR("Could not initialize event router!");
        return Failure(InitializeErrors::FailedEventRouterInitialization);
    }

    // Success!
    m_initialized = true;
    return Success();
}

bool GameFramework::Update()
{
    ASSERT(m_initialized, "Game framework has not been initialized!");

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
    ASSERT(m_initialized, "Game framework has not been initialized!");

    // Get window viewport rect.
    glm::ivec4 viewportRect = { 0, 0, m_window->GetWidth(), m_window->GetHeight() };

    // Draw game state.
    Renderer::StateRenderer::DrawParams drawParams;
    drawParams.viewportRect = viewportRect;
    drawParams.gameState = m_gameState.get();
    drawParams.cameraName = "Camera";
    m_stateRenderer->Draw(drawParams);
}

void GameFramework::SetGameState(std::shared_ptr<GameState>& gameState)
{
    ASSERT(m_initialized, "Game framework has not been initialized!");

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

std::shared_ptr<GameState> GameFramework::GetGameState()
{
    ASSERT(m_initialized, "Game framework has not been initialized!");
    return m_gameState;
}
