/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Game/SceneRenderer.hpp"
#include "Game/BaseScene.hpp"
#include "Engine.hpp"
using namespace Game;

SceneRenderer::SceneRenderer() :
    m_engine(nullptr),
    m_initialized(false)
{
}

SceneRenderer::~SceneRenderer()
{
}

SceneRenderer::SceneRenderer(SceneRenderer&& other)
{
    // Call the assignment operator.
    *this = std::move(other);
}

SceneRenderer& SceneRenderer::operator=(SceneRenderer&& other)
{
    // Swap class members.
    std::swap(m_engine, other.m_engine);
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool SceneRenderer::Initialize(Engine::Root* engine)
{
    LOG() << "Initializing scene renderer..." << LOG_INDENT();

    // Make sure class instance has not been already initialized.
    ASSERT(!m_initialized, "Scene renderer has already been initialized!");

    // Reset class instance if initialization fails.
    SCOPE_GUARD_IF(!m_initialized, *this = SceneRenderer());

    // Validate and save engine reference.
    if(engine == nullptr)
    {
        LOG_ERROR() << "Invalid argument - \"engine\" is nullptr!";
        return false;
    }

    m_engine = engine;

    // Success!
    return m_initialized = true;
}

void SceneRenderer::DrawScene(Scene* scene, const SceneDrawParams& drawParams)
{
    ASSERT(m_initialized, "Scene renderer has not been initialized yet!");
 
    // Make sure the scene pointer is valid.
    if(scene == nullptr)
        return;

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

    // Check if scene is a base scene.
    BaseScene* baseScene = dynamic_cast<BaseScene*>(scene);

    if(baseScene)
    {
        // #todo: Draw the scene components.
    }
}
