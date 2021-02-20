/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Editor/Precompiled.hpp"
#include "Editor/EditorShell.hpp"
#include <Core/ServiceStorage.hpp>
#include <Core/PerformanceMetrics.hpp>
#include <System/Window.hpp>
using namespace Editor;

namespace
{
    const char* CreateError = "Failed to create editor shell instance! {}";
    const char* CreateModulesError = "Failed to create editor shell modules! {}";
}

EditorShell::EditorShell() = default;
EditorShell::~EditorShell() = default;

EditorShell::CreateResult EditorShell::Create(const CreateFromParams& params)
{
    CHECK_ARGUMENT_OR_RETURN(params.services != nullptr,
        Common::Failure(CreateErrors::InvalidArgument));

    Core::PerformanceMetrics* performanceMetrics = params.services->GetPerformanceMetrics();
    System::Window* window = params.services->GetWindow();

    auto instance = std::unique_ptr<EditorShell>(new EditorShell());
    instance->m_performanceMetrics = performanceMetrics;
    instance->m_window = window;

    if(!instance->CreateModules(params.services))
    {
        LOG_ERROR(CreateError, "Could not create editor modules.");
        return Common::Failure(CreateErrors::FailedModuleCreation);
    }

    LOG_SUCCESS("Created editor shell instance.");
    return Common::Success(std::move(instance));
}

bool Editor::EditorShell::CreateModules(const Core::ServiceStorage* services)
{
    // Input manager editor.
    InputManagerEditor::CreateFromParams inputManagerEditorParams;
    inputManagerEditorParams.services = services;

    m_inputManagerEditor = InputManagerEditor::Create(inputManagerEditorParams).UnwrapOr(nullptr);
    if(m_inputManagerEditor == nullptr)
    {
        LOG_ERROR(CreateModulesError, "Could not create input manager editor module.");
        return false;
    }

    // Game instance editor.
    GameInstanceEditor::CreateFromParams gameInstanceEditorParams;
    gameInstanceEditorParams.services = services;

    m_gameInstanceEditor = GameInstanceEditor::Create(gameInstanceEditorParams).UnwrapOr(nullptr);
    if(m_gameInstanceEditor == nullptr)
    {
        LOG_ERROR(CreateModulesError, "Could not create game instance editor module.");
        return false;
    }

    return true;
}

void EditorShell::Display(float timeDelta)
{
    if(m_showDemoWindow)
    {
        ImGui::ShowDemoWindow(&m_showDemoWindow);
    }

    DisplayMenuBar();
    DisplayFramerate();

    m_inputManagerEditor->Display(timeDelta);
    m_gameInstanceEditor->Display(timeDelta);
}

void EditorShell::DisplayMenuBar()
{
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
}

void EditorShell::DisplayFramerate()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0.0f, 0.0f));

    ImGui::SetNextWindowPos(ImVec2(4.0f, m_window->GetHeight() - 4.0f),
        ImGuiCond_Always, ImVec2(0.0f, 1.0f));
    ImGui::SetNextWindowBgAlpha(0.0f);

    ImGuiWindowFlags flags = 0;
    flags |= ImGuiWindowFlags_NoMove;
    flags |= ImGuiWindowFlags_NoTitleBar;
    flags |= ImGuiWindowFlags_NoResize;
    flags |= ImGuiWindowFlags_AlwaysAutoResize;
    flags |= ImGuiWindowFlags_NoSavedSettings;
    flags |= ImGuiWindowFlags_NoFocusOnAppearing;
    flags |= ImGuiWindowFlags_NoNav;

    if(ImGui::Begin("Framerate Counter Button", 0, flags))
    {
        if(ImGui::Button(fmt::format("FPS: {:.0f} ({:.2f} ms)",
            m_performanceMetrics->GetFrameRate(), m_performanceMetrics->GetFrameTime()).c_str()))
        {
        }
    }
    ImGui::End();

    ImGui::PopStyleVar(4);
}

