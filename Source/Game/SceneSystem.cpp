/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Game/SceneSystem.hpp"
#include "Game/Scene.hpp"
using namespace Game;

SceneSystem::SceneSystem() :
    m_engine(nullptr),
    m_currentScene(nullptr),
    m_initialized(false)
{
}

SceneSystem::~SceneSystem()
{
}

SceneSystem::SceneSystem(SceneSystem&& other) :
    SceneSystem()
{
    // Call the move assignment.
    *this = std::move(other);
}

SceneSystem& SceneSystem::operator=(SceneSystem&& other)
{
    // Swap class members.
    std::swap(m_engine, other.m_engine);
    std::swap(m_currentScene, other.m_currentScene);
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool SceneSystem::Initialize(Engine::Root* engine)
{
    LOG() << "Initializing scene system...";

    // Make sure that this instance has not been initialized yet.
    VERIFY(!m_initialized, "Scene system has already been initialized!");

    // Validates arguments.
    if(engine == nullptr)
    {
        LOG_ERROR() << "Invalid argument - \"engine\" is null!";
        return false;
    }

    // Save engine reference.
    m_engine = engine;

    // Success!
    return m_initialized = true;
}

void SceneSystem::ChangeScene(std::shared_ptr<Scene> scene)
{
    ASSERT(m_initialized, "Scene system has not been initialized yet!");

    // Inform about scene being changed.
    const char* oldSceneName = m_currentScene ? m_currentScene->GetName() : "None";
    const char* newSceneName = scene ? scene->GetName() : "None";

    LOG_INFO() << "Changing scene from \"" << oldSceneName << "\" to \"" << newSceneName << "\".";

    // Notify previous scene about the change.
    if(m_currentScene)
    {
        m_currentScene->OnExit();
    }
    
    // Change the current scene.
    m_currentScene = scene;

    // Notify new scene about the change.
    if(m_currentScene)
    {
        m_currentScene->OnEnter();
    }
}

void Game::SceneSystem::Update(float timeDelta)
{
    ASSERT(m_initialized, "Scene system has not been initialized yet!");

    // Update the current scene.
    if(m_currentScene)
    {
        m_currentScene->OnUpdate(timeDelta);
    }
}

void Game::SceneSystem::Draw(float timeAlpha)
{
    ASSERT(m_initialized, "Scene system has not been initialized yet!");

    // Draw the current scene.
    if(m_currentScene)
    {
        m_currentScene->OnDraw(timeAlpha);
    }
}

bool Game::SceneSystem::HasCustomEditor() const
{
    ASSERT(m_initialized, "Scene system has not been initialized yet!");

    // Check if the current scene has a custom editor implemented.
    if(m_currentScene)
    {
        return m_currentScene->HasCustomEditor();
    }

    return false;
}
