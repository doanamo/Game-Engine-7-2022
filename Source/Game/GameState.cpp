/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Game/GameState.hpp"
#include "Engine.hpp"
using namespace Game;

GameState::GameState() :
    m_engine(nullptr),
    m_initialized(false)
{
}

GameState::~GameState()
{
}

GameState::GameState(GameState&& other)
{
    *this = std::move(other);
}

GameState& GameState::operator=(GameState&& other)
{
    std::swap(entitySystem, other.entitySystem);
    std::swap(componentSystem, other.componentSystem);
    std::swap(identitySystem, other.identitySystem);

    std::swap(spriteSystem, other.spriteSystem);

    std::swap(m_engine, other.m_engine);
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

    // Initialize the sprite system.
    // Updates sprites and their animations.
    if(!spriteSystem.Initialize(&componentSystem))
    {
        LOG_ERROR() << "Could not initialize sprite system!";
        return false;
    }

    // Success!
    return m_initialized = true;
}

void GameState::Update(float timeDelta)
{
    ASSERT(m_initialized, "Game state has not been initialized!");

    // Process entity commands.
    entitySystem.ProcessCommands();

    // Update the sprite animation system.
    spriteSystem.Update(timeDelta);
}

Engine::Root* GameState::GetEngine() const
{
    return m_engine;
}
