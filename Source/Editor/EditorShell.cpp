/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Editor/EditorShell.hpp"
#include <System/Window.hpp>
using namespace Editor;

EditorShell::EditorShell() = default;
EditorShell::~EditorShell() = default;

EditorShell::CreateResult EditorShell::Create(const CreateFromParams& params)
{
    LOG("Creating editor shell...");
    LOG_SCOPED_INDENT();

    // Validate engine reference.
    CHECK_ARGUMENT_OR_RETURN(params.window != nullptr, Failure(CreateErrors::InvalidArgument));

    // Create instance.
    auto instance = std::unique_ptr<EditorShell>(new EditorShell());

    // Create input manager editor.
    InputManagerEditor::CreateFromParams inputManagerEditorParams;
    inputManagerEditorParams.window = params.window;

    instance->m_inputManagerEditor = InputManagerEditor::Create(inputManagerEditorParams).UnwrapOr(nullptr);
    if(instance->m_inputManagerEditor == nullptr)
    {
        LOG_ERROR("Could not create input manager editor!");
        return Failure(CreateErrors::FailedModuleCreation);
    }

    // Create game state editor.
    GameStateEditor::CreateFromParams gameStateEditorParams;
    gameStateEditorParams.gameFramework = params.gameFramework;

    instance->m_gameStateEditor = GameStateEditor::Create(gameStateEditorParams).UnwrapOr(nullptr);
    if(instance->m_gameStateEditor == nullptr)
    {
        LOG_ERROR("Could not create game state editor!");
        return Failure(CreateErrors::FailedModuleCreation);
    }

    // Save window reference.
    instance->m_window = params.window;

    // Success!
    return Success(std::move(instance));
}

void EditorShell::Update(float timeDelta)
{
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
            ImGui::MenuItem("Input Manager", "", &m_inputManagerEditor->mainWindowOpen, true);
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Game"))
        {
            ImGui::MenuItem("Game State", "", &m_gameStateEditor->mainWindowOpen, true);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    // Update editor modules.
    m_inputManagerEditor->Update(timeDelta);
    m_gameStateEditor->Update(timeDelta);
}
