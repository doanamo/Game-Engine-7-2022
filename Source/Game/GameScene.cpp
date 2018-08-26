/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Game/GameScene.hpp"
using namespace Game;

GameScene::GameScene() :
    m_engine(nullptr),
    m_initialized(false)
{
}

GameScene::~GameScene()
{
}

GameScene::GameScene(GameScene&& other)
{
    // Call the move assignment.
    *this = std::move(other);
}

GameScene& GameScene::operator=(GameScene&& other)
{
    // Swap class members.
    std::swap(m_entitySystem, other.m_entitySystem);
    std::swap(m_componentSystem, other.m_componentSystem);
    std::swap(m_identitySystem, other.m_identitySystem);

    std::swap(m_engine, other.m_engine);
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool GameScene::Initialize(Engine::Root* engine)
{
    LOG() << "Initializing base game scene..." << LOG_INDENT();

    // Check if base game scene class has been already initialized.
    ASSERT(!m_initialized, "Base game scene class has been already initialized!");

    // Reset class instance on initialization failure.
    SCOPE_GUARD_IF(!m_initialized, *this = GameScene());

    // Validate engine reference.
    if(engine == nullptr)
    {
        LOG_ERROR() << "Invalid argument - \"engine\" is null!";
        return false;
    }

    m_engine = engine;

    // Initialize the entity system.
    // Assigns unique identifiers that all other systems use to identify objects in a game.
    if(!m_entitySystem.Initialize())
    {
        LOG_ERROR() << "Could not initialize entity system!";
        return false;
    }

    // Initialize the component system.
    // Stores and manages components that entities have.
    if(!m_componentSystem.Initialize(m_entitySystem))
    {
        LOG_ERROR() << "Could not initialize component system!";
        return false;
    }

    // Initialize the identity system.
    // Allows readable names to be assigned to entities.
    if(!m_identitySystem.Initialize(m_entitySystem))
    {
        LOG_ERROR() << "Could not initialize identity system!";
        return false;
    }

    // Success!
    return m_initialized = true;
}

void GameScene::OnUpdate(float timeDelta)
{
    // Process entity commands.
    m_entitySystem.ProcessCommands();
}

void GameScene::OnDraw(float timeAlpha)
{
}

Engine::Root* GameScene::GetEngine() const
{
    ASSERT(m_initialized, "Base game scene class has not been initialized!");

    return m_engine;
}
