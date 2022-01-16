/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

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
            case Logger::Severity::Debug:   return ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
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

        // Console messages.
        ImGui::BeginChild("Console Messages", ImVec2(0.0f, -24.0f));
        {
            ImVec2 windowSize = ImGui::GetWindowSize();
            ImGuiWindowFlags messagesFlags = 0;

            if(!ImGui::IsPopupOpen("Context Menu"))
            {
                m_copyBuffer = std::string();
            }

            if(m_autoScroll)
            {
                messagesFlags |= ImGuiWindowFlags_NoScrollWithMouse;
            }

            if(ImGui::BeginChild("Message History", ImVec2(0.0f, 0.0f), false, messagesFlags))
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
                    ImGui::Selectable(message.text.c_str());
                    ImGui::PopStyleColor();
                    ImGui::PopTextWrapPos();

                    if(ImGui::IsMouseReleased(ImGuiPopupFlags_MouseButtonRight) &&
                        ImGui::IsItemHovered())
                    {
                        m_copyBuffer = message.text;
                        ImGui::OpenPopup("Context Menu");
                    }
                }

                if(!m_resetScroll && ImGui::GetScrollY() < ImGui::GetScrollMaxY())
                {
                    // Disable auto scroll if we scroll away.
                    // Ignore if we just re-enabled auto scrolling.
                    m_autoScroll = false;
                }

                if(m_autoScroll)
                {
                    ImGui::SetScrollHereY(1.0f);
                    m_resetScroll = false;
                }
            }
            ImGui::EndChild();

            if(ImGui::IsItemHovered())
            {
                if(ImGui::IsMouseReleased(ImGuiPopupFlags_MouseButtonRight))
                {
                    ImGui::OpenPopup("Context Menu");
                }

                if(ImGui::GetIO().MouseWheel != 0.0f)
                {
                    m_autoScroll = false;
                }
            }

            // Context menu.
            if(ImGui::BeginPopupContextItem("Context Menu"))
            {
                if(!m_copyBuffer.empty())
                {
                    if(ImGui::Selectable("Copy to clipboard"))
                    {
                        ImGui::SetClipboardText(m_copyBuffer.c_str());
                    }
                }
                else
                {
                    ImGui::TextDisabled("Copy to clipboard");
                }

                if(ImGui::MenuItem("Auto-scroll", nullptr, &m_autoScroll))
                {
                    m_resetScroll = true;
                }

                if(ImGui::MenuItem("Pause", nullptr, &m_pause))
                {
                    if(m_pause)
                    {
                        LOG_INFO("Logger history has been paused.");
                    }

                    Logger::GetGlobalHistory().SetEnabled(!m_pause);

                    if(!m_pause)
                    {
                        LOG_INFO("Logger history has been unpaused.");
                    }
                }

                if(ImGui::MenuItem("Clear"))
                {
                    Logger::GetGlobalHistory().Clear();
                }

                ImGui::Separator();
                if(ImGui::BeginMenu("Filters"))
                {
                    for(int i = 0; i < Common::StaticArraySize(m_severityFilters); ++i)
                    {
                        auto severity = static_cast<Logger::Severity::Type>(i);
                        if(severity != Logger::Severity::Invalid)
                        {
                            std::string text = fmt::format("{0} ({1})###{0}",
                                Logger::GetSeverityName(severity), stats.severityCount[i]);

                            ImGui::Checkbox(text.c_str(), &m_severityFilters[i]);
                        }
                    }

                    ImGui::Separator();

                    if(ImGui::MenuItem("Select none"))
                    {
                        for(bool& severityFilter : m_severityFilters)
                        {
                            severityFilter = false;
                        }
                    }

                    if(ImGui::MenuItem("Select all"))
                    {
                        for(bool& severityFilter : m_severityFilters)
                        {
                            severityFilter = true;
                        }
                    }

                    ImGui::EndMenu();
                }

                ImGui::EndPopup();
            }
        }
        ImGui::EndChild();

        // Console input.
        ImGui::Separator();
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

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
