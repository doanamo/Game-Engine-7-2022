/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Editor/Precompiled.hpp"
#include "Editor/EditorConsole.hpp"
#include <Common/Logger/LoggerHistory.hpp>
#include <Core/System/SystemStorage.hpp>
#include <Platform/WindowSystem.hpp>
#include <Platform/Window.hpp>
using namespace Editor;

namespace
{
    const char* LogAttachFailed = "Failed to attach editor console subsystem! {}";

    ImVec4 GetLogMessageColor(Logger::Severity::Type severity)
    {
        switch(severity)
        {
            case Logger::Severity::Trace:   return ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
            case Logger::Severity::Debug:   return ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
            case Logger::Severity::Profile: return ImVec4(0.6f, 0.2f, 1.0f, 1.0f);
            case Logger::Severity::Info:    return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
            case Logger::Severity::Success: return ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
            case Logger::Severity::Warning: return ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
            case Logger::Severity::Error:   return ImVec4(1.0f, 0.4f, 0.0f, 1.0f);
            case Logger::Severity::Fatal:   return ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

            default:
                ASSERT(false, "Unknown message severity!");
                return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        }
    }
}

EditorConsole::EditorConsole()
{
    for(bool& severity : m_severityFilters)
    {
        severity = false;
    }
}

EditorConsole::~EditorConsole() = default;

bool EditorConsole::OnAttach(const EditorSubsystemStorage& editorSubsystems)
{
    // Locate needed systems.
    auto& editorContext = editorSubsystems.Locate<EditorSubsystemContext>();
    auto& engineSystems = editorContext.GetEngineSystems();

    m_windowSystem = &engineSystems.Locate<Platform::WindowSystem>();

    return true;
}

bool EditorConsole::OnKeyboardKey(const Platform::InputEvents::KeyboardKey& event)
{
    if(event.key == Platform::KeyboardKeys::KeyTilde && event.state == Platform::InputStates::Pressed)
    {
        Toggle(!IsVisible());
        return true;
    }

    return false;
}

void EditorConsole::OnBeginInterface(float timeDelta)
{
    if(!m_windowVisible)
        return;

    const Platform::Window& window = m_windowSystem->GetWindow();

    ImGuiWindowFlags windowFlags = 0;
    windowFlags |= ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoTitleBar;
    windowFlags |= ImGuiWindowFlags_NoScrollbar;
    windowFlags |= ImGuiWindowFlags_NoScrollWithMouse;

    ImVec2 consoleMinSize;
    consoleMinSize.x = window.GetWidth();
    consoleMinSize.y = 52.0f;

    ImVec2 consoleMaxSize;
    consoleMaxSize.x = window.GetWidth();
    consoleMaxSize.y = std::max((float)window.GetHeight(), consoleMinSize.y);

    ImVec2 consoleWindowSize;
    consoleWindowSize.x = window.GetWidth();
    consoleWindowSize.y = window.GetHeight() * 0.6f;

    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(consoleWindowSize, ImGuiCond_Once);
    ImGui::SetNextWindowSizeConstraints(consoleMinSize, consoleMaxSize);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

    SCOPE_GUARD([]()
    {
        ImGui::PopStyleVar(2);
    });

    if(ImGui::Begin("Console", nullptr, windowFlags))
    {
        const Logger::History::MessageList messages = Logger::GetGlobalHistory().GetMessages();
        const Logger::History::MessageStats stats = Logger::GetGlobalHistory().GetStats();

        // Context menu.
        if(ImGui::BeginPopupContextItem("Context Menu"))
        {
            if(ImGui::Selectable("Copy to clipboard"))
            {
                std::string clipboardText;

                for(const auto& message : messages)
                {
                    clipboardText += message.text;
                }

                ImGui::SetClipboardText(clipboardText.c_str());
            }

            ImGui::MenuItem("Auto-scroll", nullptr, &m_autoScroll);
            ImGui::EndPopup();
        }

        // Console messages.
        ImGui::BeginChild("Console Messages", ImVec2(0.0f, -24.0f));
        {
            ImVec2 windowSize = ImGui::GetWindowSize();
            ImGuiWindowFlags messagesFlags = 0;

            if(m_autoScroll)
            {
                messagesFlags |= ImGuiWindowFlags_NoScrollWithMouse;
            }

            if(ImGui::BeginChild("Message History",
                ImVec2(m_optionsVisible ? -180.0f : 0, 0.0f), false, messagesFlags))
            {
                bool filterActive = std::find(std::begin(m_severityFilters),
                    std::end(m_severityFilters), true) != std::end(m_severityFilters);

                for(const auto& message : messages)
                {
                    if(filterActive)
                    {
                        ASSERT(message.severity < Logger::Severity::Count);
                        int severityID = static_cast<int>(message.severity);
                        if(!m_severityFilters[severityID])
                            continue;
                    }

                    ImGui::PushTextWrapPos(0.0f);
                    ImGui::PushStyleColor(ImGuiCol_Text, GetLogMessageColor(message.severity));
                    ImGui::TextUnformatted(message.text.c_str());
                    ImGui::PopStyleColor();
                    ImGui::PopTextWrapPos();
                }

                if(m_autoScroll)
                {
                    ImGui::SetScrollHereY(1.0f);
                }
            }
            ImGui::EndChild();

            if(m_optionsVisible)
            {
                ImGui::SameLine();
                if(ImGui::BeginChild("Console Options"),
                    ImVec2(0.0f, 0.0f), true)
                {
                    static bool temp = false;
                    ImGui::Text("Filter severity:");

                    ImGui::Indent();

                    int severityID = 0;
                    for(bool& severityFilter : m_severityFilters)
                    {
                        if(severityID != 0)
                        {
                            std::string text = fmt::format("{} ({})", Logger::GetSeverityName(
                                static_cast<Logger::Severity::Type>(severityID)),
                                stats.severityCount[severityID]);

                            ImGui::PushID(severityID);
                            ImGui::Checkbox(text.c_str(), &severityFilter);
                            ImGui::PopID();
                        }

                        severityID++;
                    }

                    ImGui::Unindent();

                    if(ImGui::Button("Filter by none"))
                    {
                        for(bool& severityFilter : m_severityFilters)
                        {
                            severityFilter = false;
                        }
                    }

                    if(ImGui::Button("Filter by all"))
                    {
                        for(bool& severityFilter : m_severityFilters)
                        {
                            severityFilter = true;
                        }
                    }

                    if(ImGui::Button("Clear history"))
                    {
                        Logger::GetGlobalHistory().Clear();
                    }
                }
                ImGui::EndChild();
            }
        }
        ImGui::EndChild();

        // Console input.
        ImGui::Separator();
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - 60.0f);

        if(ImGui::InputText("##ConsoleInput", &m_inputBuffer, ImGuiInputTextFlags_EnterReturnsTrue))
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

        // Console options toggle.
        ImGui::SameLine();

        if(ImGui::Button("Options", ImVec2(60.0f, 0.0f)))
        {
            m_optionsVisible = !m_optionsVisible;
        }
    }
    ImGui::End();
}

void EditorConsole::Toggle(bool visibility)
{
    m_windowVisible = visibility;
}

bool EditorConsole::IsVisible() const
{
    return m_windowVisible;
}
