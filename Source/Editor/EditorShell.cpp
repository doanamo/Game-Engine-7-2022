/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Editor/EditorShell.hpp"
#include <System/Window.hpp>
using namespace Editor;

EditorShell::EditorShell() = default;
EditorShell::~EditorShell() = default;

EditorShell::InitializeResult EditorShell::Initialize(const InitializeFromParams& params)
{
    LOG("Initializing editor shell...");
    LOG_SCOPED_INDENT();

    // Setup initialization guard.
    VERIFY(!m_initialized, "Instance has already been initialized!");
    SCOPE_GUARD_IF(!m_initialized, this->Reset());

    // Validate engine reference.
    CHECK_ARGUMENT_OR_RETURN(params.window != nullptr, Failure(InitializeErrors::InvalidArgument));

    m_window = params.window;

    // Initialize input manager editor.
    InputManagerEditor::InitializeFromParams inputManagerEditorParams;
    inputManagerEditorParams.window = params.window;

    if(!m_inputManagerEditor.Initialize(inputManagerEditorParams))
    {
        LOG_ERROR("Could not initialize input manager editor!");
        return Failure(InitializeErrors::FailedModuleInitialization);
    }

    // Initialize game state editor.
    GameStateEditor::InitializeFromParams gameStateEditorParams;
    gameStateEditorParams.gameFramework = params.gameFramework;

    if(!m_gameStateEditor.Initialize(gameStateEditorParams))
    {
        LOG_ERROR("Could not initialize game state editor!");
        return Failure(InitializeErrors::FailedModuleInitialization);
    }

    // Success!
    m_initialized = true;
    return Success();
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
            ImGui::MenuItem("Show Demo", "", &m_showDemoWindow, true);
            ImGui::Separator();

            if(ImGui::MenuItem("Exit"))
            {
                m_window->Close();
            }

            ImGui::EndMenu();
        }

        ImGui::Separator();

        if(ImGui::BeginMenu("System"))
        {
            ImGui::MenuItem("Input Manager", "", &m_inputManagerEditor.mainWindowOpen, true);
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Game"))
        {
            ImGui::MenuItem("Game State", "", &m_gameStateEditor.mainWindowOpen, true);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    // Update editor modules.
    m_inputManagerEditor.Update(timeDelta);
    m_gameStateEditor.Update(timeDelta);
}
