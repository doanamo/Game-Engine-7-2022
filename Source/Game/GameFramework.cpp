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

bool GameFramework::ProcessGameState(float timeDelta)
{
    // Track whether tick was processed.
    bool tickProcessed = false;

    // Acquire current game state.
    if(auto currentState = m_stateMachine.GetState())
    {
        // Lazy initialized render time alpha.
        float timeAlpha = 1.0f;

        // Call game state update method.
        currentState->Update(timeDelta);

        // Request game instance tick.
        if(auto gameInstance = currentState->GetGameInstance())
        {
            // Call game state tick method if tick was processed.
            if(gameInstance->Tick(*m_timer))
            {
                currentState->Tick(gameInstance->tickTimer->GetLastTickSeconds());
                tickProcessed = true;
            }

            // Pass game instance to game renderer.
            glm::ivec4 viewportRect = { 0, 0, m_window->GetWidth(), m_window->GetHeight() };

            Renderer::GameRenderer::DrawParams drawParams;
            drawParams.viewportRect = viewportRect;
            drawParams.gameInstance = gameInstance;
            drawParams.cameraName = "Camera";
            m_gameRenderer->Draw(drawParams);

            // Retrieve proper time alpha based on last tick.
            timeAlpha = gameInstance->tickTimer->GetAlphaSeconds();
        }

        // Call game state draw method.
        currentState->Draw(timeAlpha);
    }

    // Return whether tick was processed.
    return tickProcessed;
}

bool GameFramework::ChangeGameState(std::shared_ptr<GameState> gameState)
{
    // Make sure we are not changing into current game state.
    if(gameState == m_stateMachine.GetState())
    {
        LOG_WARNING("Attempted to change into current game state!");
        return false;
    }

    // Change into new game state.
    if(!m_stateMachine.ChangeState(gameState))
        return false;

    // Notify listeners about game state transition.
    events.gameStateChanged.Dispatch(gameState);

    // State transition succeeded.
    return true;
}

bool GameFramework::HasGameState() const
{
    return m_stateMachine.HasState();
}
