/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Editor/EditorConsole.hpp"
#include <Logger/History.hpp>
#include <System/Window.hpp>
using namespace Editor;

EditorConsole::CreateResult EditorConsole::Create(const CreateFromParams& params)
{
    LOG("Creating editor console...");
    LOG_SCOPED_INDENT();

    // Validate engine reference.
    CHECK_ARGUMENT_OR_RETURN(params.services != nullptr, Common::Failure(CreateErrors::InvalidArgument));

    // Create instance.
    auto instance = std::unique_ptr<EditorConsole>(new EditorConsole());

    // Save window service reference.
    instance->m_window = params.services->GetWindow();

    // Test message colors.
    #if 0
        LOG_TRACE("Test console trace message!");
        LOG_DEBUG("Test console debug message!");
        LOG_INFO("Test console info message!");
        LOG_WARNING("Test console warning message!");
        LOG_ERROR("Test console error message!");
        LOG_FATAL("Test console fatal message!");
    #endif

    // Success!
    return Common::Success(std::move(instance));
}

EditorConsole::EditorConsole() = default;
EditorConsole::~EditorConsole() = default;

void EditorConsole::Update(float timeDelta)
{
    if(!m_visible)
        return;

    // Display console window.
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

    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImVec2(m_window->GetWidth(), m_window->GetHeight() * 0.6f), ImGuiCond_Once);
    ImGui::SetNextWindowSizeConstraints(consoleMinSize, consoleMaxSize);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

    if(ImGui::Begin("Console", nullptr, windowFlags))
    {
        ImVec2 windowSize = ImGui::GetWindowSize();

        // Make copy of message buffer to avoid race conditions.
        // This will be very slow if there is a lot of messages.
        const auto messages = Logger::GetGlobalHistory().GetMessages();

        // Console message history.
        ImGuiWindowFlags messagesFlags = 0;

        if(m_autoScroll)
        {
            messagesFlags |= ImGuiWindowFlags_NoScrollWithMouse;
        }

        if(ImGui::BeginChild("Console Messages", ImVec2(0.0f, windowSize.y - 40.0f), false, messagesFlags))
        {
            // Print all messages.
            for(const auto& message : messages)
            {
                switch(message.severity)
                {
                case Logger::Severity::Trace:
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
                    break;

                case Logger::Severity::Debug:
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
                    break;

                case Logger::Severity::Info:
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                    break;

                case Logger::Severity::Warning:
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
                    break;

                case Logger::Severity::Error:
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.0f, 1.0f));
                    break;

                case Logger::Severity::Fatal:
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                    break;

                default:
                    ASSERT(false, "Invalid message severity!");
                    continue;
                }

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

                for(const auto& message : messages)
                {
                    clipboardText += message.text;
                }

                ImGui::SetClipboardText(clipboardText.c_str());
            }

            ImGui::MenuItem("Auto-scroll", nullptr, &m_autoScroll);
            ImGui::EndPopup();
        }

        ImGui::Separator();

        // Console text input.
        ImGui::PushItemWidth(-1);

        if(ImGui::InputText("Console Input", &m_inputBuffer, ImGuiInputTextFlags_EnterReturnsTrue))
        {
            // Regain focus on the input field.
            ImGui::SetKeyboardFocusHere();

            // Print entered console command.
            LOG_INFO("> {}", m_inputBuffer.c_str());

            // Parse console input.
            // ParseInput(inputString);

            // Clear console input.
            m_inputBuffer.clear();

            // Scroll message list.
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
