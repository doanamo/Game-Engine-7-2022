/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Game/Precompiled.hpp"
#include "Game/GameFramework.hpp"
#include "Game/GameInstance.hpp"
#include "Game/TickTimer.hpp"
#include <Core/ServiceStorage.hpp>
#include <System/Window.hpp>
using namespace Game;

GameFramework::GameFramework() = default;
GameFramework::~GameFramework() = default;

bool GameFramework::OnAttach(const Core::ServiceStorage* services)
{
    // Retrieve required services.
    m_timer = services->Locate<System::Timer>();
    if(!m_timer)
    {
        LOG_ERROR("Failed to locate timer service!");
        return false;
    }

    // Success!
    return true;
}

GameFramework::ProcessGameStateResults GameFramework::ProcessGameState(const float timeDelta)
{
    // Acquire current state and its parts.
    std::shared_ptr<GameState> currentState = m_stateMachine.GetState();
    TickTimer* tickTimer = currentState ? currentState->GetTickTimer() : nullptr;
    GameInstance* gameInstance = currentState ? currentState->GetGameInstance() : nullptr;

    // Track whether tick was processed.
    bool tickProcessed = false;

    // Process current game state.
    if(currentState)
    {
        // Process tick timer.
        if(tickTimer)
        {
            tickTimer->Advance(*m_timer);
        }

        // Inform about tick being requested.
        events.tickRequested.Dispatch();

        // Process game tick.
        // Tick may be processed multiple times if behind the schedule.
        while(!tickTimer || tickTimer->Tick())
        {
            // Determine tick time.
            float tickTime = tickTimer ? tickTimer->GetLastTickSeconds() : timeDelta;

            // Tick game instance.
            if(gameInstance)
            {
                gameInstance->Tick(tickTime);
            }

            // Call game state tick method.
            currentState->Tick(tickTime);

            // Inform that tick has been processed.
            events.tickProcessed.Dispatch(tickTime);

            // Mark tick as processed.
            tickProcessed = true;

            // Tick only once if there is no tick timer.
            if(!tickTimer)
                break;
        }

        // Call game state update method.
        currentState->Update(timeDelta);

        // Determine time alpha.
        float timeAlpha = tickTimer ? tickTimer->GetAlphaSeconds() : 1.0f;

        // Request game instance to be drawn.
        if(gameInstance)
        {
            events.drawGameInstance.Dispatch(gameInstance, timeAlpha);
        }

        // Call game state draw method.
        currentState->Draw(timeAlpha);
    }

    // Return whether tick was processed.
    return tickProcessed
        ? ProcessGameStateResults::TickedAndUpdated
        : ProcessGameStateResults::UpdatedOnly;
}

GameFramework::ChangeGameStateResult GameFramework::ChangeGameState(std::shared_ptr<GameState> gameState)
{
    // Make sure we are not changing into current game state.
    if(gameState == m_stateMachine.GetState())
    {
        LOG_WARNING("Attempted to change into current game state!");
        return Common::Failure(ChangeGameStateErrors::AlreadyCurrent);
    }

    // Change into new game state.
    if(!m_stateMachine.ChangeState(gameState))
        return Common::Failure(ChangeGameStateErrors::FailedTransition);

    // Notify listeners about game state transition.
    events.gameStateChanged.Dispatch(gameState);

    // State transition succeeded.
    return Common::Success();
}

bool GameFramework::HasGameState() const
{
    return m_stateMachine.HasState();
}
