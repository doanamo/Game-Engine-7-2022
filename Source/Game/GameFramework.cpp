/*
    Copyright (c) 2018-2019 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Game/GameFramework.hpp"
#include "Game/GameState.hpp"
#include "System/Window.hpp"
#include "Renderer/StateRenderer.hpp"
#include "Engine/Root.hpp"
using namespace Game;

GameFramework::GameFramework() :
    m_engine(nullptr),
    m_initialized(false)
{
}

GameFramework::~GameFramework()
{
}

GameFramework::GameFramework(GameFramework&& other) :
    GameFramework()
{
    *this = std::move(other);
}

GameFramework& GameFramework::operator=(GameFramework&& other)
{
    std::swap(m_engine, other.m_engine);
    std::swap(m_eventRouter, other.m_eventRouter);
    std::swap(m_gameState, other.m_gameState);
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool GameFramework::Initialize(Engine::Root* engine)
{
    LOG() << "Initializing game framework..." << LOG_INDENT();

    // Make sure class instance has not been initialized yet.
    ASSERT(!m_initialized, "Game framework has already been initialized!");

    // Initialization scope guard.
    SCOPE_GUARD_IF(!m_initialized, *this = GameFramework());

    // Save engine reference.
    if(engine == nullptr)
    {
        LOG_ERROR() << "Invalid argument - \"engine\" is null!";
        return false;
    }

    m_engine = engine;

    // Initialize the event router.
    // Listens and replicates event to the current game state.
    if(!m_eventRouter.Initialize(engine))
    {
        LOG_ERROR() << "Could not initialize event router!";
        return false;
    }

    // Success!
    return m_initialized = true;
}

bool GameFramework::Update()
{
    ASSERT(m_initialized, "Game framework has not been initialized!");

    // Get current timer.
    System::Timer& timer = m_engine->GetTimer();

    // Update game state.
    if(m_gameState)
    {
        return m_gameState->Update(timer);
    }

    return false;
}

void GameFramework::Draw()
{
    ASSERT(m_initialized, "Game framework has not been initialized!");

    // Get window viewport rect.
    System::Window& window = m_engine->GetWindow();
    glm::ivec4 viewportRect = { 0, 0, window.GetWidth(), window.GetHeight() };

    // Get state renderer.
    Renderer::StateRenderer& stateRenderer = m_engine->GetStateRenderer();

    // Draw game state.
    Renderer::StateRenderer::DrawParams drawParams;
    drawParams.viewportRect = viewportRect;
    drawParams.gameState = m_gameState.get();
    drawParams.cameraName = "Camera";
    stateRenderer.Draw(drawParams);
}

void GameFramework::SetGameState(std::shared_ptr<GameState>& gameState)
{
    ASSERT(m_initialized, "Game framework has not been initialized!");

    // Make sure we are not setting the same game state.
    if(gameState == m_gameState)
    {
        LOG_WARNING() << "Attempted to change game state into the current one!";
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
