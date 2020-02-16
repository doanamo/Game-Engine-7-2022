/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Editor/Modules/InputManagerEditor.hpp"
#include "System/InputDefinitions.hpp"
#include "Engine/Root.hpp"
using namespace Editor;

InputManagerEditor::InputManagerEditor() :
    mainWindowOpen(false),
    m_engine(nullptr),
    m_incomingEventFreeze(false),
    m_incomingWindowFocus(false),
    m_incomingKeyboardKey(false),
    m_incomingKeyboardKeyPress(false),
    m_incomingKeyboardKeyRelease(false),
    m_incomingKeyboardKeyRepeat(false),
    m_incomingTextInput(false),
    m_incomingMouseButton(false),
    m_incomingMouseButtonPress(false),
    m_incomingMouseButtonRelease(false),
    m_incomingMouseButtonRepeat(false),
    m_incomingMouseScroll(false),
    m_incomingCursorPosition(false),
    m_incomingCursorEnter(false),
    m_incomingEventLogSize(100),
    m_incomingEventCounter(0),
    m_initialized(false)
{
    m_windowFocusReceiver.Bind<InputManagerEditor, &InputManagerEditor::OnWindowFocus>(this);
    m_keyboardKeyReceiver.Bind<InputManagerEditor, &InputManagerEditor::OnKeyboardKey>(this);
    m_textInputReceiver.Bind<InputManagerEditor, &InputManagerEditor::OnTextInput>(this);
    m_mouseButtonReceiver.Bind<InputManagerEditor, &InputManagerEditor::OnMouseButton>(this);
    m_mouseScrollReceiver.Bind<InputManagerEditor, &InputManagerEditor::OnMouseScroll>(this);
    m_cursorPositionReceiver.Bind<InputManagerEditor, &InputManagerEditor::OnCursorPosition>(this);
    m_cursorEnterReceiver.Bind<InputManagerEditor, &InputManagerEditor::OnCursorEnter>(this);
}

InputManagerEditor::~InputManagerEditor()
{
}

InputManagerEditor::InputManagerEditor(InputManagerEditor&& other) :
    InputManagerEditor()
{
    *this = std::move(other);
}

InputManagerEditor& InputManagerEditor::operator=(InputManagerEditor&& other)
{
    std::swap(mainWindowOpen, other.mainWindowOpen);
    std::swap(m_engine, other.m_engine);

    std::swap(m_windowFocusReceiver, other.m_windowFocusReceiver);
    std::swap(m_keyboardKeyReceiver, other.m_keyboardKeyReceiver);
    std::swap(m_textInputReceiver, other.m_textInputReceiver);
    std::swap(m_mouseButtonReceiver, other.m_mouseButtonReceiver);
    std::swap(m_mouseScrollReceiver, other.m_mouseScrollReceiver);
    std::swap(m_cursorPositionReceiver, other.m_cursorPositionReceiver);
    std::swap(m_cursorEnterReceiver, other.m_cursorEnterReceiver);

    std::swap(m_incomingEventFreeze, other.m_incomingEventFreeze);
    std::swap(m_incomingWindowFocus, other.m_incomingWindowFocus);
    std::swap(m_incomingKeyboardKey, other.m_incomingKeyboardKey);
    std::swap(m_incomingKeyboardKeyPress, other.m_incomingKeyboardKeyPress);
    std::swap(m_incomingKeyboardKeyRelease, other.m_incomingKeyboardKeyRelease);
    std::swap(m_incomingKeyboardKeyRepeat, other.m_incomingKeyboardKeyRepeat);
    std::swap(m_incomingTextInput, other.m_incomingTextInput);
    std::swap(m_incomingMouseButton, other.m_incomingMouseButton);
    std::swap(m_incomingMouseButtonPress, other.m_incomingMouseButtonPress);
    std::swap(m_incomingMouseButtonRelease, other.m_incomingMouseButtonRelease);
    std::swap(m_incomingMouseButtonRepeat, other.m_incomingMouseButtonRepeat);
    std::swap(m_incomingMouseScroll, other.m_incomingMouseScroll);
    std::swap(m_incomingCursorPosition, other.m_incomingCursorPosition);
    std::swap(m_incomingCursorEnter, other.m_incomingCursorEnter);

    std::swap(m_incomingEventLog, other.m_incomingEventLog);
    std::swap(m_incomingEventCounter, other.m_incomingEventCounter);
    
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool InputManagerEditor::Initialize(Engine::Root* engine)
{
    LOG_SCOPED_INDENT();
    LOG() << "Initializing input manager editor...";

    // Make sure class instance is not already initialized.
    ASSERT(!m_initialized, "Input manager editor has already been initialized!");

    // Initialization scope guard.
    SCOPE_GUARD_IF(!m_initialized, *this = InputManagerEditor());

    // Validate engine reference.
    if(engine == nullptr)
    {
        LOG_ERROR() << "Invalid argument - \"engine\" is null!";
        return false;
    }

    m_engine = engine;

    // Subscribe to incoming window events, same as InputManager does.
    bool subscriptionResults = true;

    subscriptionResults &= m_keyboardKeyReceiver.Subscribe(m_engine->GetWindow().events.keyboardKey);
    subscriptionResults &= m_textInputReceiver.Subscribe(m_engine->GetWindow().events.textInput);
    subscriptionResults &= m_windowFocusReceiver.Subscribe(m_engine->GetWindow().events.focus);
    subscriptionResults &= m_mouseButtonReceiver.Subscribe(m_engine->GetWindow().events.mouseButton);
    subscriptionResults &= m_mouseScrollReceiver.Subscribe(m_engine->GetWindow().events.mouseScroll);
    subscriptionResults &= m_cursorPositionReceiver.Subscribe(m_engine->GetWindow().events.cursorPosition);
    subscriptionResults &= m_cursorEnterReceiver.Subscribe(m_engine->GetWindow().events.cursorEnter);

    if(!subscriptionResults)
    {
        LOG_ERROR() << "Failed to subscribe to event receivers!";
        return false;
    }

    // Success!
    return m_initialized = true;
}

void InputManagerEditor::Update(float timeDelta)
{
    ASSERT(m_initialized, "Input manager editor has not been initialized!");

    // Show main window.
    if(mainWindowOpen)
    {
        ImGui::SetNextWindowSizeConstraints(ImVec2(200.0f, 0.0f), ImVec2(FLT_MAX, FLT_MAX));

        if(ImGui::Begin("Input Manager", &mainWindowOpen, ImGuiWindowFlags_AlwaysAutoResize))
        {
            if(ImGui::CollapsingHeader("Events"))
            {
                if(ImGui::TreeNode("Incoming"))
                {
                    ImGui::BeginChild("Incoming Event Log", ImVec2(300, 340), true);
                    for(const std::string& eventText : m_incomingEventLog)
                    {
                        ImGui::TextWrapped("%s", eventText.c_str());
                    }
                    ImGui::SetScrollHere(1.0f);
                    ImGui::EndChild();

                    ImGui::SameLine();
                    ImGui::BeginChild("Event Type Toggle", ImVec2(150, 0), false);

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

                    ImGui::EndChild();
                    ImGui::TreePop();
                }
            }
        }

        ImGui::End();
    }
}

void InputManagerEditor::AddIncomingEventLog(std::string text)
{
    ASSERT(m_initialized, "Input manager editor has not been initialized!");

    // Check if event log has been frozen by the user.
    if(m_incomingEventFreeze)
        return;

    // Maintain maximum log size.
    while(m_incomingEventLog.size() > m_incomingEventLogSize)
    {
        m_incomingEventLog.pop_front();
    }

    // Increment the event counter.
    m_incomingEventCounter = ++m_incomingEventCounter % 1000;

    std::stringstream textStream;
    textStream << std::setw(3) << std::setfill('0') << m_incomingEventCounter;
    textStream << ": " << text;

    // Add new event text to log deque.
    m_incomingEventLog.push_back(textStream.str());
}

void InputManagerEditor::OnWindowFocus(const System::Window::Events::Focus& event)
{
    ASSERT(m_initialized, "Input manager editor has not been initialized!");

    // Add event text log for window focus.
    if(m_incomingWindowFocus)
    {
        std::stringstream eventText;
        eventText << "Window Focus\n";
        eventText << "  Focused: " << (event.focused ? "True" : "False");

        AddIncomingEventLog(eventText.str());
    }
}

bool InputManagerEditor::OnKeyboardKey(const System::Window::Events::KeyboardKey& event)
{
    ASSERT(m_initialized, "Input manager editor has not been initialized!");

    // Filter keyboard key events.
    if((event.action == GLFW_PRESS && !m_incomingKeyboardKeyPress) ||
        (event.action == GLFW_RELEASE && !m_incomingKeyboardKeyRelease) ||
        (event.action == GLFW_REPEAT && !m_incomingKeyboardKeyRepeat))
    {
        // Skip keyboard key events we want to filter.
        return false;
    }

    // Add event text log for keyboard key.
    if(m_incomingKeyboardKey)
    {
        std::stringstream eventText;
        eventText << "Keyboard Key\n";
        eventText << "  Key: " << event.key << "\n";
        eventText << "  Scancode: " << event.scancode << "\n";
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

    // Do not capture input.
    return false;
}

bool InputManagerEditor::OnTextInput(const System::Window::Events::TextInput& event)
{
    ASSERT(m_initialized, "Input manager editor has not been initialized!");

    // Add event text log for text input.
    if(m_incomingTextInput)
    {
        std::stringstream eventText;
        eventText << "Text Input\n";
        eventText << "  Character: " << event.utf32Character;

        AddIncomingEventLog(eventText.str());
    }

    // Do not capture input.
    return false;
}

bool InputManagerEditor::OnMouseButton(const System::Window::Events::MouseButton& event)
{
    ASSERT(m_initialized, "Input manager editor has not been initialized!");

    // Filter mouse button events.
    if((event.action == GLFW_PRESS && !m_incomingMouseButtonPress) ||
        (event.action == GLFW_RELEASE && !m_incomingMouseButtonRelease) ||
        (event.action == GLFW_REPEAT && !m_incomingMouseButtonRepeat))
    {
        // Skip mouse button events we want to filter.
        return false;
    }

    // Add event text log for mouse button.
    if(m_incomingMouseButton)
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

    // Do not capture input.
    return false;
}

bool InputManagerEditor::OnMouseScroll(const System::Window::Events::MouseScroll& event)
{
    ASSERT(m_initialized, "Input manager editor has not been initialized!");

    // Add event text log for mouse scroll.
    if(m_incomingMouseScroll)
    {
        std::stringstream eventText;
        eventText << "Mouse Scroll\n";
        eventText << "  Offset: " << event.offset;

        AddIncomingEventLog(eventText.str());
    }

    // Do not capture input.
    return false;
}

void InputManagerEditor::OnCursorPosition(const System::Window::Events::CursorPosition& event)
{
    ASSERT(m_initialized, "Input manager editor has not been initialized!");

    // Add event text log for cursor position.
    if(m_incomingCursorPosition)
    {
        std::stringstream eventText;
        eventText << "Cursor Position\n";
        eventText << "  Position: " << event.x << " / " << event.y;

        AddIncomingEventLog(eventText.str());
    }
}

void InputManagerEditor::OnCursorEnter(const System::Window::Events::CursorEnter& event)
{
    ASSERT(m_initialized, "Input manager editor has not been initialized!");

    // Add event text log for cursor enter.
    if(m_incomingCursorEnter)
    {
        std::stringstream eventText;
        eventText << "Cursor Enter\n";
        eventText << "  Entered: " << (event.entered ? "True" : "False");

        AddIncomingEventLog(eventText.str());
    }
}
