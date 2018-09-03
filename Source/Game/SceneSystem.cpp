/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Game/SceneSystem.hpp"
#include "Game/Scene.hpp"
#include "Game/EntitySystem.hpp"
#include "Engine.hpp"
using namespace Game;

SceneSystem::SceneSystem() :
    m_engine(nullptr),
    m_currentScene(nullptr),
    m_initialized(false)
{
}

SceneSystem::~SceneSystem()
{
    // Exit the current scene before destruction.
    if(m_currentScene)
    {
        m_currentScene->OnExit();
    }
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

void SceneSystem::DrawScene(Scene* scene, const SceneDrawParams& drawParams)
{
    ASSERT(m_initialized, "Scene system has not been initialized yet!");

    // Draw scene with provided parameters.
    if(scene)
    {
        // Push the render state.
        auto& renderState = m_engine->renderContext.PushState();
        SCOPE_GUARD(m_engine->renderContext.PopState());

        // Setup the drawing viewport.
        renderState.Viewport(
            drawParams.viewportRect.x,
            drawParams.viewportRect.y,
            drawParams.viewportRect.z,
            drawParams.viewportRect.w
        );

        // Call the drawing method.
        scene->OnDraw(drawParams);
    }
}

void SceneSystem::Update(float timeDelta)
{
    ASSERT(m_initialized, "Scene system has not been initialized yet!");

    // Update the current scene.
    if(m_currentScene)
    {
        m_currentScene->OnUpdate(timeDelta);
    }
}

void SceneSystem::Draw(float timeAlpha)
{
    ASSERT(m_initialized, "Scene system has not been initialized yet!");

    // Draw the current scene.
    if(m_currentScene)
    {
        SceneDrawParams drawParams;
        drawParams.viewportRect.z = m_engine->window.GetWidth();
        drawParams.viewportRect.w = m_engine->window.GetHeight();
        drawParams.timeAlpha = timeAlpha;

        this->DrawScene(m_currentScene.get(), drawParams);
    }
}
