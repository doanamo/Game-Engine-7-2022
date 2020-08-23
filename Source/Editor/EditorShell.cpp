/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Editor/EditorShell.hpp"
#include <Core/PerformanceMetrics.hpp>
#include <System/Window.hpp>
using namespace Editor;

EditorShell::EditorShell() = default;
EditorShell::~EditorShell() = default;

EditorShell::CreateResult EditorShell::Create(const CreateFromParams& params)
{
    LOG("Creating editor shell...");
    LOG_SCOPED_INDENT();

    // Validate engine reference.
    CHECK_ARGUMENT_OR_RETURN(params.services != nullptr, Common::Failure(CreateErrors::InvalidArgument));

    // Acquire window service.
    Core::PerformanceMetrics* performanceMetrics = params.services->GetPerformanceMetrics();
    System::Window* window = params.services->GetWindow();

    // Create instance.
    auto instance = std::unique_ptr<EditorShell>(new EditorShell());

    // Create input manager editor.
    InputManagerEditor::CreateFromParams inputManagerEditorParams;
    inputManagerEditorParams.services = params.services;

    instance->m_inputManagerEditor = InputManagerEditor::Create(inputManagerEditorParams).UnwrapOr(nullptr);
    if(instance->m_inputManagerEditor == nullptr)
    {
        LOG_ERROR("Could not create input manager editor!");
        return Common::Failure(CreateErrors::FailedModuleCreation);
    }

    // Create game instance editor.
    GameInstanceEditor::CreateFromParams gameInstanceEditorParams;
    gameInstanceEditorParams.services = params.services;

    instance->m_gameInstanceEditor = GameInstanceEditor::Create(gameInstanceEditorParams).UnwrapOr(nullptr);
    if(instance->m_gameInstanceEditor == nullptr)
    {
        LOG_ERROR("Could not create game instance editor!");
        return Common::Failure(CreateErrors::FailedModuleCreation);
    }

    // Save window reference.
    instance->m_performanceMetrics = performanceMetrics;
    instance->m_window = window;

    // Success!
    return Common::Success(std::move(instance));
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
            ImGui::MenuItem("Game Framework", "", &m_gameInstanceEditor->mainWindowOpen, true);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0.0f, 0.0f));

    ImGui::SetNextWindowPos(ImVec2(0.0f, m_window->GetHeight() - 4.0f), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
    ImGui::SetNextWindowBgAlpha(0.0f);

    if(ImGui::Begin("Framerate Overlay Button", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
    {
        if(ImGui::Button(fmt::format("FPS: {:.0f} ({:.2f} ms)", m_performanceMetrics->GetFrameRate(), m_performanceMetrics->GetFrameTime()).c_str()))
        {
        }

        ImGui::End();
    }

    ImGui::PopStyleVar(3);

    // Update editor modules.
    m_inputManagerEditor->Update(timeDelta);
    m_gameInstanceEditor->Update(timeDelta);
}
