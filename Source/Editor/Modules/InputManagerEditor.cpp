/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Editor/Precompiled.hpp"
#include "Editor/Modules/InputManagerEditor.hpp"
#include <Core/SystemStorage.hpp>
#include <Core/EngineSystem.hpp>
#include <System/Window.hpp>
#include <System/InputManager.hpp>
using namespace Editor;

namespace
{
    const char* AttachError = "Failed to attach input manager editor module! {}";
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

bool InputManagerEditor::OnAttach(const Core::SystemStorage<EditorModule>& editorModules)
{
    // Retrieve needed systems.
    auto* editorContext = editorModules.Locate<EditorModuleContext>();
    auto& engineSystems = editorContext->GetEngineSystems();

    m_window = engineSystems.Locate<System::Window>();
    if(!m_window)
    {
        LOG_ERROR(AttachError, "Could not retrieve window.");
        return false;
    }

    auto* inputManager = engineSystems.Locate<System::InputManager>();
    if(!inputManager)
    {
        LOG_ERROR(AttachError, "Could not retrieve input manager.");
        return false;
    }

    // Subscribe to input events.
    System::InputState& inputState = inputManager->GetInputState();

    bool subscriptionResults = true;
    subscriptionResults &= m_window->events.Subscribe(m_windowFocusReceiver);
    subscriptionResults &= inputState.events.Subscribe(m_keyboardKeyReceiver);
    subscriptionResults &= inputState.events.Subscribe(m_textInputReceiver);
    subscriptionResults &= inputState.events.Subscribe(m_mouseButtonReceiver);
    subscriptionResults &= inputState.events.Subscribe(m_mouseScrollReceiver);
    subscriptionResults &= inputState.events.Subscribe(m_cursorPositionReceiver);
    subscriptionResults &= inputState.events.Subscribe(m_cursorEnterReceiver);

    if(!subscriptionResults)
    {
        LOG_ERROR(AttachError, "Could not subscribe to window events.");
        return false;
    }

    // Success!
    return true;
}

void InputManagerEditor::OnDisplay(float timeDelta)
{
    if(!m_isOpen)
        return;

    ImGui::SetNextWindowSizeConstraints(ImVec2(200.0f, 0.0f), ImVec2(FLT_MAX, FLT_MAX));
    if(ImGui::Begin("Input Manager", &m_isOpen, ImGuiWindowFlags_AlwaysAutoResize))
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

void InputManagerEditor::OnDisplayMenuBar()
{
    if(ImGui::BeginMenu("System"))
    {
        ImGui::MenuItem("Input Manager", "", &m_isOpen, true);
        ImGui::EndMenu();
    }
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

void InputManagerEditor::OnWindowFocus(const System::WindowEvents::Focus& event)
{
    if(m_incomingWindowFocus)
    {
        std::stringstream eventText;
        eventText << "Window Focus\n";
        eventText << "  Focused: " << (event.focused ? "True" : "False");

        AddIncomingEventLog(eventText.str());
    }
}

bool InputManagerEditor::OnTextInput(const System::InputEvents::TextInput& event)
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

bool InputManagerEditor::OnKeyboardKey(const System::InputEvents::KeyboardKey& event)
{
    if(m_incomingKeyboardKey)
    {
        bool processEvent = false;
        processEvent |= System::IsInputStatePressed(event.state) && m_incomingKeyboardKeyPress;
        processEvent |= System::IsInputStateReleased(event.state) && m_incomingKeyboardKeyRelease;

        if(System::IsInputStateRepeating(event.state))
        {
            processEvent &= m_incomingKeyboardKeyRepeat;
        }

        if(processEvent)
        {
            std::stringstream eventText;
            eventText << "Keyboard Key\n";
            eventText << "  Key: " << event.key << "\n";
            eventText << "  State: ";

            switch(event.state)
            {
            case System::InputStates::Pressed:
                eventText << "Pressed";
                break;

            case System::InputStates::PressedReleased:
                eventText << "Pressed (Released)";
                break;

            case System::InputStates::PressedRepeat:
                eventText << "Pressed (Repeat)";
                break;

            case System::InputStates::Released:
                eventText << "Released";
                break;

            case System::InputStates::ReleasedRepeat:
                eventText << "Released (Repeat)";
                break;

            default:
                eventText << "Unknown";
                break;
            }

            eventText << "\n";
            eventText << "  Modifiers: ";

            if(event.modifiers)
            {
                if(event.modifiers & System::KeyboardModifiers::Shift)
                    eventText << "Shift ";

                if(event.modifiers & System::KeyboardModifiers::Ctrl)
                    eventText << "Ctrl ";

                if(event.modifiers & System::KeyboardModifiers::Alt)
                    eventText << "Alt ";

                if(event.modifiers & System::KeyboardModifiers::Super)
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

bool InputManagerEditor::OnMouseButton(const System::InputEvents::MouseButton& event)
{
    if(m_incomingMouseButton)
    {
        bool processEvent = false;
        processEvent |= System::IsInputStatePressed(event.state) && m_incomingMouseButtonPress;
        processEvent |= System::IsInputStateReleased(event.state) && m_incomingMouseButtonRelease;
        
        if(System::IsInputStateRepeating(event.state))
        {
            processEvent &= m_incomingKeyboardKeyRepeat;
        }

        if(processEvent)
        {
            std::stringstream eventText;
            eventText << "Mouse Button\n";
            eventText << "  Button: " << event.button << "\n";
            eventText << "  Action: ";

            switch(event.state)
            {
            case System::InputStates::Pressed:
                eventText << "Pressed";
                break;

            case System::InputStates::PressedReleased:
                eventText << "Pressed (Released)";
                break;

            case System::InputStates::PressedRepeat:
                eventText << "Pressed (Repeat)";
                break;

            case System::InputStates::Released:
                eventText << "Released";
                break;

            case System::InputStates::ReleasedRepeat:
                eventText << "Released (Repeat)";
                break;

            default:
                eventText << "Unknown";
                break;
            }

            eventText << "\n";
            eventText << "  Modifiers: ";

            if(event.modifiers)
            {
                if(event.modifiers & System::KeyboardModifiers::Shift)
                    eventText << "Shift ";

                if(event.modifiers & System::KeyboardModifiers::Ctrl)
                    eventText << "Ctrl ";

                if(event.modifiers & System::KeyboardModifiers::Alt)
                    eventText << "Alt ";

                if(event.modifiers & System::KeyboardModifiers::Super)
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

bool InputManagerEditor::OnMouseScroll(const System::InputEvents::MouseScroll& event)
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

void InputManagerEditor::OnCursorPosition(const System::InputEvents::CursorPosition& event)
{
    if(m_incomingCursorPosition)
    {
        std::stringstream eventText;
        eventText << "Cursor Position\n";
        eventText << "  Position: " << event.x << " / " << event.y;

        AddIncomingEventLog(eventText.str());
    }
}

void InputManagerEditor::OnCursorEnter(const System::InputEvents::CursorEnter& event)
{
    if(m_incomingCursorEnter)
    {
        std::stringstream eventText;
        eventText << "Cursor Enter\n";
        eventText << "  Entered: " << (event.entered ? "True" : "False");

        AddIncomingEventLog(eventText.str());
    }
}
