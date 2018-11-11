/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Game/BaseScene.hpp"
#include "Engine.hpp"
using namespace Game;

BaseScene::BaseScene() :
    m_engine(nullptr),
    m_initialized(false)
{
}

BaseScene::~BaseScene()
{
}

BaseScene::BaseScene(BaseScene&& other)
{
    *this = std::move(other);
}

BaseScene& BaseScene::operator=(BaseScene&& other)
{
    std::swap(m_engine, other.m_engine);
    std::swap(m_gameState, other.m_gameState);
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool BaseScene::Initialize(Engine::Root* engine)
{
    LOG() << "Initializing base scene..." << LOG_INDENT();

    // Check if class instance has already been initialized.
    VERIFY(!m_initialized, "Base scene has already been initialized!");

    // Reset class instance on initialization failure.
    SCOPE_GUARD_IF(!m_initialized, *this = BaseScene());

    // Validate engine reference.
    if(engine == nullptr || !engine->IsInitialized())
    {
        LOG_ERROR() << "Invalid argument - \"engine\" is invalid!";
        return false;
    }

    m_engine = engine;

    // Initialize the game state.
    if(!m_gameState.Initialize(engine))
    {
        LOG_ERROR() << "Could not initialize game state!";
        return false;
    }

    // Success!
    return m_initialized = true;
}

void BaseScene::OnUpdate(float timeDelta)
{
    ASSERT(m_initialized, "Base scene has not been initialized!");

    // Update the game state.
    m_gameState.Update(timeDelta);
}

void BaseScene::OnDraw(const SceneDrawParams& drawParams)
{
    ASSERT(m_initialized, "Base scene has not been initialized!");
}

Engine::Root* BaseScene::GetEngine() const
{
    ASSERT(m_initialized, "Base scene has not been initialized!");

    return m_engine;
}

GameState& BaseScene::GetGameState()
{
    ASSERT(m_initialized, "Base scene has not been initialized!");

    return m_gameState;
}
