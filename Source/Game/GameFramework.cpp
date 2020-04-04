/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Game/GameFramework.hpp"
#include "Game/GameState.hpp"
#include <System/Window.hpp>
#include <Renderer/StateRenderer.hpp>
using namespace Game;

GameFramework::GameFramework(GameFramework&& other) :
    GameFramework()
{
    *this = std::move(other);
}

GameFramework& GameFramework::operator=(GameFramework&& other)
{
    std::swap(m_timer, other.m_timer);
    std::swap(m_window, other.m_window);
    std::swap(m_stateRenderer, other.m_stateRenderer);
    std::swap(m_eventRouter, other.m_eventRouter);
    std::swap(m_gameState, other.m_gameState);
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool GameFramework::Initialize(const InitializeFromParams& params)
{
    LOG("Initializing game framework...");
    LOG_SCOPED_INDENT();

    // Make sure class instance has not been initialized yet.
    ASSERT(!m_initialized, "Game framework has already been initialized!");

    // Initialization scope guard.
    SCOPE_GUARD_IF(!m_initialized, *this = GameFramework());

    // Save engine reference.
    if(params.timer == nullptr)
    {
        LOG_ERROR("Invalid argument - \"timer\" is null!");
        return false;
    }

    if(params.window == nullptr)
    {
        LOG_ERROR("Invalid argument - \"window\" is null!");
        return false;
    }

    if(params.stateRenderer == nullptr)
    {
        LOG_ERROR("Invalid argument - \"stateRenderer\" is null!");
        return false;
    }

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
        return false;
    }

    // Success!
    return m_initialized = true;
}

bool GameFramework::Update()
{
    ASSERT(m_initialized, "Game framework has not been initialized!");

    // Update game state.
    if(m_gameState)
    {
        return m_gameState->Update(*m_timer);
    }

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
    return m_gameState;
}
