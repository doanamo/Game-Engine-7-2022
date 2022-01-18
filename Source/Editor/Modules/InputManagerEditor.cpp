/*
    Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Editor/Modules/InputManagerEditor.hpp"
#include <Core/System/SystemStorage.hpp>
#include <Core/System/EngineSystem.hpp>
#include <Platform/WindowSystem.hpp>
#include <Platform/Window.hpp>
#include <Platform/InputManager.hpp>
using namespace Editor;

namespace
{
    const char* LogAttachFailed = "Failed to attach input manager editor module! {}";

    const char* GetInputStateText(Platform::InputStates::Type state)
    {
        switch(state)
        {
        case Platform::InputStates::Pressed:
            return "Pressed";

        case Platform::InputStates::PressedReleased:
            return "PressedReleased";

        case Platform::InputStates::PressedRepeat:
            return "PressedRepeat";

        case Platform::InputStates::Released:
            return "Released";

        case Platform::InputStates::ReleasedRepeat:
            return "ReleasedRepeat";
        }

        return "Unknown";
    }

    std::string GetInputModifiersText(Platform::KeyboardModifiers::Type modifiers)
    {
        std::string text;

        if(modifiers & Platform::KeyboardModifiers::Shift)
            text += "Shift ";

        if(modifiers & Platform::KeyboardModifiers::Ctrl)
            text += "Ctrl ";

        if(modifiers & Platform::KeyboardModifiers::Alt)
            text += "Alt ";

        if(modifiers & Platform::KeyboardModifiers::Super)
            text += "Super ";

        if(text.empty())
            text = "None";

        return text;
    }
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
    auto& editorContext = editorModules.Locate<EditorModuleContext>();
    auto& engineSystems = editorContext.GetEngineSystems();

    auto& m_windowSystem = engineSystems.Locate<Platform::WindowSystem>();
    auto& inputManager = engineSystems.Locate<Platform::InputManager>();

    // Subscribe to input events.
    bool subscriptionResults = true;

    Platform::Window& window = m_windowSystem.GetWindow();
    Platform::InputState& inputState = inputManager.GetInputState();
    subscriptionResults &= window.events.Subscribe(m_windowFocusReceiver);
    subscriptionResults &= inputState.events.Subscribe(m_keyboardKeyReceiver);
    subscriptionResults &= inputState.events.Subscribe(m_textInputReceiver);
    subscriptionResults &= inputState.events.Subscribe(m_mouseButtonReceiver);
    subscriptionResults &= inputState.events.Subscribe(m_mouseScrollReceiver);
    subscriptionResults &= inputState.events.Subscribe(m_cursorPositionReceiver);
    subscriptionResults &= inputState.events.Subscribe(m_cursorEnterReceiver);

    if(!subscriptionResults)
    {
        LOG_ERROR(LogAttachFailed, "Could not subscribe to window events.");
        return false;
    }

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
                // Event log panel.
                ImGui::BeginChild("Event Log", ImVec2(300, 340), true);
                {
                    for(const std::string& eventText : m_incomingEventLog)
                    {
                        ImGui::TextWrapped("%s", eventText.c_str());
                    }
                    ImGui::SetScrollHereY(1.0f);
                }
                ImGui::EndChild();

                // Event control panel.
                ImGui::SameLine();
                ImGui::BeginChild("Event Control", ImVec2(150, 0), false);
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

    // Remove oldest entry.
    while(m_incomingEventLog.size() > m_incomingEventLogSize)
    {
        m_incomingEventLog.pop_front();
    }

    m_incomingEventCounter = ++m_incomingEventCounter % 1000;

    // Add new formatted entry.
    m_incomingEventLog.push_back(fmt::format("{:0>4}: {}", m_incomingEventCounter, text));
}

void InputManagerEditor::OnWindowFocus(const Platform::WindowEvents::Focus& event)
{
    if(m_incomingWindowFocus)
    {
        AddIncomingEventLog(fmt::format("Window Focus\n  Focused: {}",
            event.focused ? "True" : "False"));
    }
}

bool InputManagerEditor::OnTextInput(const Platform::InputEvents::TextInput& event)
{
    if(m_incomingTextInput)
    {
        AddIncomingEventLog(fmt::format("Text Input\n  Character: {}", 
            event.utf32Character));
    }

    return false;
}

bool InputManagerEditor::OnKeyboardKey(const Platform::InputEvents::KeyboardKey& event)
{
    if(m_incomingKeyboardKey)
    {
        bool processEvent = false;
        processEvent |= Platform::IsInputStatePressed(event.state) && m_incomingKeyboardKeyPress;
        processEvent |= Platform::IsInputStateReleased(event.state) && m_incomingKeyboardKeyRelease;

        if(Platform::IsInputStateRepeating(event.state))
        {
            processEvent &= m_incomingKeyboardKeyRepeat;
        }

        if(processEvent)
        {
            AddIncomingEventLog(
                fmt::format("Keyboard Key\n  Key: {}\n  State: {}\n  Modifiers: {}",
                event.key, GetInputStateText(event.state), GetInputModifiersText(event.modifiers)));
        }
    }

    return false;
}

bool InputManagerEditor::OnMouseButton(const Platform::InputEvents::MouseButton& event)
{
    if(m_incomingMouseButton)
    {
        bool processEvent = false;
        processEvent |= Platform::IsInputStatePressed(event.state) && m_incomingMouseButtonPress;
        processEvent |= Platform::IsInputStateReleased(event.state) && m_incomingMouseButtonRelease;
        
        if(Platform::IsInputStateRepeating(event.state))
        {
            processEvent &= m_incomingKeyboardKeyRepeat;
        }

        if(processEvent)
        {
            AddIncomingEventLog(
                fmt::format("Mouse Button\n  Button: {}\n  State: {}\n  Modifiers: {}",
                event.button, GetInputStateText(event.state),
                GetInputModifiersText(event.modifiers)));
        }
    }

    return false;
}

bool InputManagerEditor::OnMouseScroll(const Platform::InputEvents::MouseScroll& event)
{
    if(m_incomingMouseScroll)
    {
        AddIncomingEventLog(fmt::format("Mouse Scroll\n  Offset: {}", event.offset));
    }

    return false;
}

void InputManagerEditor::OnCursorPosition(const Platform::InputEvents::CursorPosition& event)
{
    if(m_incomingCursorPosition)
    {
        AddIncomingEventLog(fmt::format("Cursor Position\n  Position: {} / {}",
            event.x, event.y));
    }
}

void InputManagerEditor::OnCursorEnter(const Platform::InputEvents::CursorEnter& event)
{
    if(m_incomingCursorEnter)
    {
        AddIncomingEventLog(fmt::format("Cursor Enter\n  Entered: {}",
            event.entered ? "True" : "False"));
    }
}
