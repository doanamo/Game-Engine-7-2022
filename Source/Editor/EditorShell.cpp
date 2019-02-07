/*
    Copyright (c) 2018-2019 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Editor/EditorShell.hpp"
#include "System/Window.hpp"
#include "Engine/Root.hpp"
using namespace Editor;

EditorShell::EditorShell() :
    m_engine(nullptr),
    m_showDemoWindow(false),
    m_initialized(false)
{
}

EditorShell::~EditorShell()
{
}

EditorShell::EditorShell(EditorShell&& other) :
    EditorShell()
{
    *this = std::move(other);
}

EditorShell& EditorShell::operator=(EditorShell&& other)
{
    std::swap(m_engine, other.m_engine);
    std::swap(m_gameStateEditor, other.m_gameStateEditor);
    std::swap(m_showDemoWindow, other.m_showDemoWindow);
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool EditorShell::Initialize(Engine::Root* engine)
{
    LOG() << "Initializing editor shell..." << LOG_INDENT();

    // Make sure instance is not initialized.
    ASSERT(!m_initialized, "Editor shell instance is already initialized!");

    // Create an initialization guard.
    SCOPE_GUARD_IF(!m_initialized, *this = EditorShell());

    // Validate engine reference.
    if(engine == nullptr)
    {
        LOG_ERROR() << "Invalid argument - \"engine\" is null!";
        return false;
    }

    m_engine = engine;

    // Initialize the game state editor.
    if(!m_gameStateEditor.Initialize(engine))
    {
        LOG_ERROR() << "Could not initialize game state editor!";
        return false;
    }

    // Success!
    return m_initialized = true;
}

void EditorShell::Update(float timeDelta)
{
    ASSERT(m_initialized, "Editor shell has not been initialized!");

    // Show demo window.
    if(m_showDemoWindow)
    {
        ImGui::ShowDemoWindow(&m_showDemoWindow);
    }

    // Show main menu bar.
    if(ImGui::BeginMainMenuBar())
    {
        if(ImGui::BeginMenu("Editor"))
        {
            if(ImGui::MenuItem("Show Demo"))
            {
                m_showDemoWindow = true;
            }

            ImGui::Separator();

            if(ImGui::MenuItem("Exit"))
            {
                m_engine->GetWindow().Close();
            }

            ImGui::EndMenu();
        }

        ImGui::Separator();

        if(ImGui::BeginMenu("System"))
        {
            if(ImGui::MenuItem("Window"))
            {
            }

            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Game"))
        {
            if(ImGui::MenuItem("Game State"))
            {
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    // Update editor modules.
    m_gameStateEditor.Update(timeDelta);
}
