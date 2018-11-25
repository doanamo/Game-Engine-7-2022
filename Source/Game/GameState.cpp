/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Game/GameState.hpp"
#include "Engine.hpp"
using namespace Game;

GameState::GameState() :
    m_engine(nullptr),
    m_updateTime(1.0f / 10.0f),
    m_initialized(false)
{
}

GameState::~GameState()
{
    // Notify about game state instance being destructed.
    events.instanceDestruction.Dispatch();
}

GameState::GameState(GameState&& other) :
    GameState()
{
    *this = std::move(other);
}

GameState& GameState::operator=(GameState&& other)
{
    std::swap(updateTimer, other.updateTimer);

    std::swap(entitySystem, other.entitySystem);
    std::swap(componentSystem, other.componentSystem);
 
    std::swap(identitySystem, other.identitySystem);
    std::swap(interpolationSystem, other.interpolationSystem);
    std::swap(spriteSystem, other.spriteSystem);

    std::swap(m_engine, other.m_engine);

    std::swap(m_updateTime, other.m_updateTime);

    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool GameState::Initialize(Engine::Root* engine)
{
    LOG() << "Initializing game state..." << LOG_INDENT();

    // Check if class instance has already been initialized.
    VERIFY(!m_initialized, "Game state has already been initialized!");

    // Reset class instance on initialization failure.
    SCOPE_GUARD_IF(!m_initialized, *this = GameState());

    // Validate engine reference.
    if(engine == nullptr || !engine->IsInitialized())
    {
        LOG_ERROR() << "Invalid argument - \"engine\" is invalid!";
        return false;
    }

    m_engine = engine;

    // Initialize the entity system.
    // Assigns unique identifiers that all other systems use to identify objects in a game.
    if(!entitySystem.Initialize())
    {
        LOG_ERROR() << "Could not initialize entity system!";
        return false;
    }

    // Initialize the component system.
    // Stores and manages components that entities have.
    if(!componentSystem.Initialize(&entitySystem))
    {
        LOG_ERROR() << "Could not initialize component system!";
        return false;
    }

    // Initialize the identity system.
    // Allows readable names to be assigned to entities.
    if(!identitySystem.Initialize(&entitySystem))
    {
        LOG_ERROR() << "Could not initialize identity system!";
        return false;
    }

    // Initialize the interpolation system.
    // Controls how and when entities are interpolated.
    if(!interpolationSystem.Initialize(&componentSystem))
    {
        LOG_ERROR() << "Could not initialize interpolation system!";
        return false;
    }

    // Initialize the sprite system.
    // Updates sprites and their animations.
    if(!spriteSystem.Initialize(&componentSystem))
    {
        LOG_ERROR() << "Could not initialize sprite system!";
        return false;
    }

    // Initialize the update timer.
    if(!updateTimer.Initialize())
    {
        LOG_ERROR() << "Could not initialize update timer!";
        return false;
    }

    // Success!
    return m_initialized = true;
}

bool GameState::Update(const System::Timer& timer)
{
    ASSERT(m_initialized, "Game state has not been initialized!");

    // Tick the update timer along with the application timer.
    updateTimer.Tick(timer);

    // Return flag indicating if state was updated.
    bool stateUpdated = false;

    // Main game state update loop.
    const float updateTime = m_updateTime;

    if(this->updateTimer.Update(updateTime))
    {
        // Process entity commands.
        entitySystem.ProcessCommands();

        // Update the interpolation system.
        interpolationSystem.Update(updateTime);

        // Update the sprite animation system.
        spriteSystem.Update(updateTime);

        // State has been updated at least once.
        stateUpdated = true;
    }

    // Return whether state could be updated.
    return stateUpdated;
}

Engine::Root* GameState::GetEngine() const
{
    return m_engine;
}

void GameState::SetUpdateTime(float updateTime)
{
    m_updateTime = updateTime;
}

float GameState::GetUpdateTime() const
{
    return m_updateTime;
}
