/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Editor/Precompiled.hpp"
#include "Editor/Modules/InputManagerEditor.hpp"
#include <Core/ServiceStorage.hpp>
#include <System/InputDefinitions.hpp>
using namespace Editor;

namespace
{
    const char* CreateError = "Failed to create input manager editor module! {}";
}

InputManagerEditor::InputManagerEditor()
{
    m_windowFocusReceiver.Bind<InputManagerEditor, &InputManagerEditor::OnWindowFocus>(this);
    m_keyboardKeyReceiver.Bind<InputManagerEditor, &InputManagerEditor::OnKeyboardKey>(this);
    m_textInputReceiver.Bind<InputManagerEditor, &InputManagerEditor::OnTextInput>(this);
    m_mouseButtonReceiver.Bind<InputManagerEditor, &InputManagerEditor::OnMouseButton>(this);
    m_mouseScrollReceiver.Bind<InputManagerEditor, &InputManagerEditor::OnMouseScroll>(this);
    m_cursorPositionReceiver.Bind<InputManagerEditor, &InputManagerEditor::OnCursorPosition>(this);
    m_cursorEnterReceiver.Bind<InputManagerEditor, &InputManagerEditor::OnCursorEnter>(this);
}

InputManagerEditor::~InputManagerEditor() = default;

InputManagerEditor::CreateResult InputManagerEditor::Create(const CreateFromParams& params)
{
    CHECK_ARGUMENT_OR_RETURN(params.services != nullptr,
        Common::Failure(CreateErrors::InvalidArgument));

    auto instance = std::unique_ptr<InputManagerEditor>(new InputManagerEditor());
    instance->m_window = params.services->GetWindow();

    if(!instance->SubscribeEvents())
    {
        LOG_ERROR(CreateError, "Could not subscribe to window events.");
        return Common::Failure(CreateErrors::FailedEventSubscription);
    }

    LOG_SUCCESS("Created input manager editor module.");
    return Common::Success(std::move(instance));
}


bool InputManagerEditor::SubscribeEvents()
{
    bool subscriptionResults = true;
    subscriptionResults &= m_keyboardKeyReceiver.Subscribe(m_window->events.keyboardKey);
    subscriptionResults &= m_textInputReceiver.Subscribe(m_window->events.textInput);
    subscriptionResults &= m_windowFocusReceiver.Subscribe(m_window->events.focus);
    subscriptionResults &= m_mouseButtonReceiver.Subscribe(m_window->events.mouseButton);
    subscriptionResults &= m_mouseScrollReceiver.Subscribe(m_window->events.mouseScroll);
    subscriptionResults &= m_cursorPositionReceiver.Subscribe(m_window->events.cursorPosition);
    subscriptionResults &= m_cursorEnterReceiver.Subscribe(m_window->events.cursorEnter);
    return subscriptionResults;
}

void InputManagerEditor::Display(float timeDelta)
{
    if(!mainWindowOpen)
        return;

    ImGui::SetNextWindowSizeConstraints(ImVec2(200.0f, 0.0f), ImVec2(FLT_MAX, FLT_MAX));
    if(ImGui::Begin("Input Manager", &mainWindowOpen, ImGuiWindowFlags_AlwaysAutoResize))
    {
        if(ImGui::CollapsingHeader("Events"))
        {
            if(ImGui::TreeNode("Incoming"))
            {
                ImGui::BeginChild("Incoming Event Log", ImVec2(300, 340), true);
                {
                    for(const std::string& eventText : m_incomingEventLog)
                    {
                        ImGui::TextWrapped("%s", eventText.c_str());
                    }
                    ImGui::SetScrollHere(1.0f);
                }
                ImGui::EndChild();

                ImGui::SameLine();
                ImGui::BeginChild("Event Type Toggle", ImVec2(150, 0), false);
                {
                    if(ImGui::Button("Clear"))
                    {
                        m_incomingEventLog.clear();
                    }

                    ImGui::SameLine();
                    if(ImGui::Button(m_incomingEventFreeze ? "Unfreeze" : "Freeze"))
                    {
                        m_incomingEventFreeze = !m_incomingEventFreeze;
                    }

                    ImGui::Checkbox("Window Focus", &m_incomingWindowFocus);
                    ImGui::Checkbox("Keyboard Key", &m_incomingKeyboardKey);
                    ImGui::Indent();
                    ImGui::Checkbox("Press##KeyboardKeyPress", &m_incomingKeyboardKeyPress);
                    ImGui::Checkbox("Release##KeyboardKeyRelease", &m_incomingKeyboardKeyRelease);
                    ImGui::Checkbox("Repeat##KeyboardKeyRepeat", &m_incomingKeyboardKeyRepeat);
                    ImGui::Unindent();
                    ImGui::Checkbox("Text Input", &m_incomingTextInput);
                    ImGui::Checkbox("Mouse Button", &m_incomingMouseButton);
                    ImGui::Indent();
                    ImGui::Checkbox("Press##MouseButtonPress", &m_incomingMouseButtonPress);
                    ImGui::Checkbox("Release##MouseButtonRelease", &m_incomingMouseButtonRelease);
                    ImGui::Checkbox("Repeat##MouseButtonRepeat", &m_incomingMouseButtonRepeat);
                    ImGui::Unindent();
                    ImGui::Checkbox("Mouse Scroll", &m_incomingMouseScroll);
                    ImGui::Checkbox("Cursor Position", &m_incomingCursorPosition);
                    ImGui::Checkbox("Cursor Enter", &m_incomingCursorEnter);
                }
                ImGui::EndChild();

                ImGui::TreePop();
            }
        }
    }

    ImGui::End();
}

void InputManagerEditor::AddIncomingEventLog(std::string text)
{
    if(m_incomingEventFreeze)
        return;

    while(m_incomingEventLog.size() > m_incomingEventLogSize)
    {
        m_incomingEventLog.pop_front();
    }

    m_incomingEventCounter = ++m_incomingEventCounter % 1000;

    std::stringstream textStream;
    textStream << std::setw(3) << std::setfill('0') << m_incomingEventCounter;
    textStream << ": " << text;

    m_incomingEventLog.push_back(textStream.str());
}

void InputManagerEditor::OnWindowFocus(const WindowEvents::Focus& event)
{
    if(m_incomingWindowFocus)
    {
        std::stringstream eventText;
        eventText << "Window Focus\n";
        eventText << "  Focused: " << (event.focused ? "True" : "False");

        AddIncomingEventLog(eventText.str());
    }
}

bool InputManagerEditor::OnTextInput(const WindowEvents::TextInput& event)
{
    if(m_incomingTextInput)
    {
        std::stringstream eventText;
        eventText << "Text Input\n";
        eventText << "  Character: " << event.utf32Character;

        AddIncomingEventLog(eventText.str());
    }

    return false;
}

bool InputManagerEditor::OnKeyboardKey(const WindowEvents::KeyboardKey& event)
{
    if(m_incomingKeyboardKey)
    {
        bool processEvent = false;
        processEvent |= event.action == GLFW_PRESS && m_incomingKeyboardKeyPress;
        processEvent |= event.action == GLFW_RELEASE && m_incomingKeyboardKeyRelease;
        processEvent |= event.action == GLFW_REPEAT && m_incomingKeyboardKeyRepeat;

        if(processEvent)
        {
            std::stringstream eventText;
            eventText << "Keyboard Key\n";
            eventText << "  Key: " << event.key << "\n";
            eventText << "  Code: " << event.scancode << "\n";
            eventText << "  Action: ";

            switch(event.action)
            {
            case GLFW_PRESS:
                eventText << "Press";
                break;

            case GLFW_RELEASE:
                eventText << "Release";
                break;

            case GLFW_REPEAT:
                eventText << "Repeat";
                break;

            default:
                eventText << "Unknown";
                break;
            }

            eventText << "\n";
            eventText << "  Modifiers: ";

            if(event.modifiers)
            {
                if(event.modifiers & GLFW_MOD_SHIFT)
                    eventText << "Shift ";

                if(event.modifiers & GLFW_MOD_CONTROL)
                    eventText << "Ctrl ";

                if(event.modifiers & GLFW_MOD_ALT)
                    eventText << "Alt ";

                if(event.modifiers & GLFW_MOD_SUPER)
                    eventText << "Super ";
            }
            else
            {
                eventText << "None";
            }

            AddIncomingEventLog(eventText.str());
        }
    }

    return false;
}

bool InputManagerEditor::OnMouseButton(const WindowEvents::MouseButton& event)
{
    if(m_incomingMouseButton)
    {
        bool processEvent = false;
        processEvent |= event.action == GLFW_PRESS && m_incomingMouseButtonPress;
        processEvent |= event.action == GLFW_RELEASE && m_incomingMouseButtonRelease;
        processEvent |= event.action == GLFW_REPEAT && m_incomingMouseButtonRepeat;

        if(processEvent)
        {
            std::stringstream eventText;
            eventText << "Mouse Button\n";
            eventText << "  Button: " << event.button << "\n";
            eventText << "  Action: ";

            switch(event.action)
            {
            case GLFW_PRESS:
                eventText << "Press";
                break;

            case GLFW_RELEASE:
                eventText << "Release";
                break;

            case GLFW_REPEAT:
                eventText << "Repeat";
                break;

            default:
                eventText << "Unknown";
                break;
            }

            eventText << "\n";
            eventText << "  Modifiers: ";

            if(event.modifiers)
            {
                if(event.modifiers & GLFW_MOD_SHIFT)
                    eventText << "Shift ";

                if(event.modifiers & GLFW_MOD_CONTROL)
                    eventText << "Ctrl ";

                if(event.modifiers & GLFW_MOD_ALT)
                    eventText << "Alt ";

                if(event.modifiers & GLFW_MOD_SUPER)
                    eventText << "Super ";
            }
            else
            {
                eventText << "None";
            }

            AddIncomingEventLog(eventText.str());
        }
    }

    return false;
}

bool InputManagerEditor::OnMouseScroll(const WindowEvents::MouseScroll& event)
{
    if(m_incomingMouseScroll)
    {
        std::stringstream eventText;
        eventText << "Mouse Scroll\n";
        eventText << "  Offset: " << event.offset;

        AddIncomingEventLog(eventText.str());
    }

    return false;
}

void InputManagerEditor::OnCursorPosition(const WindowEvents::CursorPosition& event)
{
    if(m_incomingCursorPosition)
    {
        std::stringstream eventText;
        eventText << "Cursor Position\n";
        eventText << "  Position: " << event.x << " / " << event.y;

        AddIncomingEventLog(eventText.str());
    }
}

void InputManagerEditor::OnCursorEnter(const WindowEvents::CursorEnter& event)
{
    if(m_incomingCursorEnter)
    {
        std::stringstream eventText;
        eventText << "Cursor Enter\n";
        eventText << "  Entered: " << (event.entered ? "True" : "False");

        AddIncomingEventLog(eventText.str());
    }
}
