/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Game/Precompiled.hpp"
#include "Game/GameFramework.hpp"
#include "Game/GameInstance.hpp"
#include "Game/TickTimer.hpp"
#include <Core/SystemStorage.hpp>
#include <System/Window.hpp>
#include <System/InputManager.hpp>
using namespace Game;

namespace
{
    const char* LogAttachFailed = "Failed to attach game framework! {}";
    const char* LogFinalizeFailed = "Failed to finalize game framework! {}";
}

GameFramework::GameFramework() = default;
GameFramework::~GameFramework() = default;

bool GameFramework::OnAttach(const Core::EngineSystemStorage& engineSystems)
{
    // Retrieve required engine systems.
    m_timer = engineSystems.Locate<System::Timer>();
    if(!m_timer)
    {
        LOG_ERROR(LogAttachFailed, "Could not locate timer system.");
        return false;
    }

    return true;
}

bool GameFramework::OnFinalize(const Core::EngineSystemStorage& engineSystems)
{
    // Subscribe input manager to tick processed event so it can update its input state.
    auto* inputManager = engineSystems.Locate<System::InputManager>();
    if(!inputManager)
    {
        LOG_ERROR(LogFinalizeFailed, "Could not locate input manager.");
        return false;
    }

    if(!inputManager->events.onTickProcessed.Subscribe(events.tickProcessed))
    {
        LOG_ERROR(LogFinalizeFailed, "Could not subscribe input manager to event.");
        return false;
    }

    return true;
}

void GameFramework::OnProcessFrame()
{
    // Acquire current state and its modules.
    std::shared_ptr<GameState> currentState = m_stateMachine.GetState();
    TickTimer* tickTimer = currentState ? currentState->GetTickTimer() : nullptr;
    GameInstance* gameInstance = currentState ? currentState->GetGameInstance() : nullptr;

    // Process current game state.
    if(currentState)
    {
        // Determine current update time.
        const float updateTime = m_timer->GetDeltaSeconds();

        // Process tick timer.
        if(tickTimer)
        {
            tickTimer->Advance(*m_timer);
        }

        events.tickRequested.Dispatch();

        // Process game tick. Multiple times if behind the schedule.
        while(!tickTimer || tickTimer->Tick())
        {
            // Determine tick time.
            float tickTime = tickTimer ? tickTimer->GetLastTickSeconds() : updateTime;

            // Tick game instance.
            if(gameInstance)
            {
                gameInstance->Tick(tickTime);
            }

            // Call game state tick method.
            currentState->Tick(tickTime);
            events.tickProcessed.Dispatch(tickTime);

            // Tick only once if there is no tick timer.
            if(!tickTimer)
                break;
        }

        // Call game state update method.
        currentState->Update(updateTime);
        events.updateProcessed.Dispatch(updateTime);

        // Determine alpha time for interpolation.
        float timeAlpha = tickTimer ? tickTimer->CalculateAlphaSeconds() : 1.0f;

        // Request game instance to be drawn.
        if(gameInstance)
        {
            events.drawGameInstance.Dispatch(gameInstance, timeAlpha);
        }

        // Call game state draw method.
        currentState->Draw(timeAlpha);
    }
}

GameFramework::ChangeGameStateResult GameFramework::ChangeGameState(std::shared_ptr<GameState> gameState)
{
    // Make sure we are not changing into current game state.
    if(gameState == m_stateMachine.GetState())
    {
        LOG_WARNING("Attempted to change into current game state!");
        return Common::Failure(ChangeGameStateErrors::AlreadyCurrent);
    }

    // Attempt to change into new game state.
    if(!m_stateMachine.ChangeState(gameState))
    {
        return Common::Failure(ChangeGameStateErrors::FailedTransition);
    }

    // Notify listeners about successful game state transition.
    events.gameStateChanged.Dispatch(gameState);
    return Common::Success();
}
