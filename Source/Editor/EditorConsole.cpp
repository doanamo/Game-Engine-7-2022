/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Editor/Precompiled.hpp"
#include "Editor/EditorConsole.hpp"
#include <Common/Logger/History.hpp>
#include <Core/ServiceStorage.hpp>
#include <System/Window.hpp>
using namespace Editor;

namespace
{
    ImVec4 GetLogMessageColor(Logger::Severity::Type severity)
    {
        switch(severity)
        {
        case Logger::Severity::Trace:   return ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
        case Logger::Severity::Debug:   return ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
        case Logger::Severity::Info:    return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        case Logger::Severity::Success: return ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
        case Logger::Severity::Warning: return ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
        case Logger::Severity::Error:   return ImVec4(1.0f, 0.4f, 0.0f, 1.0f);
        case Logger::Severity::Fatal:   return ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

        default:
            ASSERT(false, "Invalid message severity!");
            return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        }
    }
}

EditorConsole::CreateResult EditorConsole::Create(const CreateFromParams& params)
{
    CHECK_ARGUMENT_OR_RETURN(params.services != nullptr,
        Common::Failure(CreateErrors::InvalidArgument));

    auto instance = std::unique_ptr<EditorConsole>(new EditorConsole());
    instance->m_window = params.services->GetWindow();

    LOG_SUCCESS("Created editor console instance.");
    return Common::Success(std::move(instance));
}

EditorConsole::EditorConsole() = default;
EditorConsole::~EditorConsole() = default;

void EditorConsole::Display(float timeDelta)
{
    if(!m_visible)
        return;

    ImGuiWindowFlags windowFlags = 0;
    windowFlags |= ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoTitleBar;
    windowFlags |= ImGuiWindowFlags_NoScrollbar;
    windowFlags |= ImGuiWindowFlags_NoScrollWithMouse;

    ImVec2 consoleMinSize;
    consoleMinSize.x = m_window->GetWidth();
    consoleMinSize.y = 52.0f;

    ImVec2 consoleMaxSize;
    consoleMaxSize.x = m_window->GetWidth();
    consoleMaxSize.y = std::max((float)m_window->GetHeight(), consoleMinSize.y);

    ImVec2 consoleWindowSize;
    consoleWindowSize.x = m_window->GetWidth();
    consoleWindowSize.y = m_window->GetHeight() * 0.6f;

    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(consoleWindowSize, ImGuiCond_Once);
    ImGui::SetNextWindowSizeConstraints(consoleMinSize, consoleMaxSize);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

    if(ImGui::Begin("Console", nullptr, windowFlags))
    {
        const auto copiedMessages = Logger::GetGlobalHistory().GetMessages();

        ImVec2 windowSize = ImGui::GetWindowSize();
        ImGuiWindowFlags messagesFlags = 0;

        if(m_autoScroll)
        {
            messagesFlags |= ImGuiWindowFlags_NoScrollWithMouse;
        }

        if(ImGui::BeginChild("Console Messages",
            ImVec2(0.0f, windowSize.y - 40.0f), false, messagesFlags))
        {
            for(const auto& message : copiedMessages)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, GetLogMessageColor(message.severity));
                ImGui::TextWrapped(message.text.c_str());
                ImGui::PopStyleColor();
            }

            if(m_autoScroll)
            {
                ImGui::SetScrollHereY(1.0f);
            }
        }
        ImGui::EndChild();

        if(ImGui::BeginPopupContextItem("Console Context Menu"))
        {
            if(ImGui::Selectable("Copy to clipboard"))
            {
                std::string clipboardText;

                for(const auto& message : copiedMessages)
                {
                    clipboardText += message.text;
                }

                ImGui::SetClipboardText(clipboardText.c_str());
            }

            ImGui::MenuItem("Auto-scroll", nullptr, &m_autoScroll);
            ImGui::EndPopup();
        }

        ImGui::Separator();
        ImGui::PushItemWidth(-1);

        if(ImGui::InputText("Console Input", &m_inputBuffer, ImGuiInputTextFlags_EnterReturnsTrue))
        {
            ImGui::SetKeyboardFocusHere();

            LOG_INFO("> {}", m_inputBuffer.c_str());

            m_inputBuffer.clear();
            m_autoScroll = true;
        }

        if(ImGui::IsWindowAppearing())
        {
            ImGui::SetKeyboardFocusHere(-1);
        }

        ImGui::PopItemWidth();
    }
    ImGui::End();

    ImGui::PopStyleVar(2);
}

void EditorConsole::Toggle(bool visibility)
{
    m_visible = visibility;
}

bool EditorConsole::IsVisible() const
{
    return m_visible;
}

bool EditorConsole::OnKeyboardKey(const System::InputEvents::KeyboardKey& event)
{
    if(event.key == System::KeyboardKeys::KeyTilde
        && event.state == System::InputStates::Pressed)
    {
        Toggle(!IsVisible());
        return true;
    }

    return false;
}
