/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Editor/EditorSystem.hpp"
#include "Game/GameState.hpp"
#include "Engine/Root.hpp"
using namespace Editor;

namespace
{
    // Callback function for setting clipboard text.
    void SetClipboardTextCallback(void* userData, const char* text)
    {
        ASSERT(userData != nullptr, "User data argument is nullptr!");
        glfwSetClipboardString((GLFWwindow*)userData, text);
    }

    // Callback function for getting clipboard text.
    const char* GetClipboardTextCallback(void* userData)
    {
        ASSERT(userData != nullptr, "User data argument is nullptr!");
        return glfwGetClipboardString((GLFWwindow*)userData);
    }
}

EditorSystem::EditorSystem() :
    m_engine(nullptr),
    m_interface(nullptr),
    m_showDemoWindow(false),
    m_initialized(false)
{
    // Bind event receivers.
    m_receiverCursorPosition.Bind<EditorSystem, &EditorSystem::CursorPositionCallback>(this);
    m_receiverMouseButton.Bind<EditorSystem, &EditorSystem::MouseButtonCallback>(this);
    m_receiverMouseScroll.Bind<EditorSystem, &EditorSystem::MouseScrollCallback>(this);
    m_receiverKeyboardKey.Bind<EditorSystem, &EditorSystem::KeyboardKeyCallback>(this);
    m_receiverTextInput.Bind<EditorSystem, &EditorSystem::TextInputCallback>(this);
}

EditorSystem::~EditorSystem()
{
    this->DestroyContext();
}

EditorSystem::EditorSystem(EditorSystem&& other) :
    EditorSystem()
{
    *this = std::move(other);
}

EditorSystem& EditorSystem::operator=(EditorSystem&& other)
{
    std::swap(m_engine, other.m_engine);
    std::swap(m_interface, other.m_interface);
    std::swap(m_showDemoWindow, other.m_showDemoWindow);

    std::swap(m_receiverCursorPosition, other.m_receiverCursorPosition);
    std::swap(m_receiverMouseButton, other.m_receiverMouseButton);
    std::swap(m_receiverMouseScroll, other.m_receiverMouseScroll);
    std::swap(m_receiverKeyboardKey, other.m_receiverKeyboardKey);
    std::swap(m_receiverTextInput, other.m_receiverTextInput);

    std::swap(m_editorRenderer, other.m_editorRenderer);
    std::swap(m_editorGameState, other.m_editorGameState);

    std::swap(m_initialized, other.m_initialized);

    return *this;
}

void EditorSystem::DestroyContext()
{
    // Destroy the user interface context.
    if(m_interface)
    {
        ImGui::DestroyContext(m_interface);
        m_interface = nullptr;
    }
}

bool EditorSystem::Initialize(Engine::Root* engine)
{
    LOG() << "Initializing editor system..." << LOG_INDENT();

    // Check if the instance is already initialized.
    VERIFY(!m_initialized, "Editor system instance is already initialized!");

    // Reset class instance if initialization fails.
    SCOPE_GUARD_IF(!m_initialized, *this = EditorSystem());

    // Validate engine reference.
    if(engine == nullptr)
    {
        LOG_ERROR() << "Invalid argument - \"engine\" is invalid!";
        return false;
    }

    m_engine = engine;

    // Create ImGui context.
    LOG() << "Creating interface context...";

    m_interface = ImGui::CreateContext();

    if(m_interface == nullptr)
    {
        LOG_ERROR() << "Failed to initialize user interface context!";
        return false;
    }

    // Setup user interface.
    ImGuiIO& io = ImGui::GetIO();

    // Disable writing of INI config in the working directory.
    // This file would hold the layout of windows, but we plan
    // on doing it differently and reading it elsewhere.
    io.IniFilename = nullptr;

    // Setup interface input.
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    io.KeyMap[ImGuiKey_Tab] = System::KeyboardKeys::KeyTab;
    io.KeyMap[ImGuiKey_LeftArrow] = System::KeyboardKeys::KeyLeft;
    io.KeyMap[ImGuiKey_RightArrow] = System::KeyboardKeys::KeyRight;
    io.KeyMap[ImGuiKey_UpArrow] = System::KeyboardKeys::KeyUp;
    io.KeyMap[ImGuiKey_DownArrow] = System::KeyboardKeys::KeyDown;
    io.KeyMap[ImGuiKey_PageUp] = System::KeyboardKeys::KeyPageUp;
    io.KeyMap[ImGuiKey_PageDown] = System::KeyboardKeys::KeyPageDown;
    io.KeyMap[ImGuiKey_Home] = System::KeyboardKeys::KeyHome;
    io.KeyMap[ImGuiKey_End] = System::KeyboardKeys::KeyEnd;
    io.KeyMap[ImGuiKey_Insert] = System::KeyboardKeys::KeyInsert;
    io.KeyMap[ImGuiKey_Delete] = System::KeyboardKeys::KeyDelete;
    io.KeyMap[ImGuiKey_Backspace] = System::KeyboardKeys::KeyBackspace;
    io.KeyMap[ImGuiKey_Space] = System::KeyboardKeys::KeySpace;
    io.KeyMap[ImGuiKey_Enter] = System::KeyboardKeys::KeyEnter;
    io.KeyMap[ImGuiKey_Escape] = System::KeyboardKeys::KeyEscape;
    io.KeyMap[ImGuiKey_A] = System::KeyboardKeys::KeyA;
    io.KeyMap[ImGuiKey_C] = System::KeyboardKeys::KeyC;
    io.KeyMap[ImGuiKey_V] = System::KeyboardKeys::KeyV;
    io.KeyMap[ImGuiKey_X] = System::KeyboardKeys::KeyX;
    io.KeyMap[ImGuiKey_Y] = System::KeyboardKeys::KeyY;
    io.KeyMap[ImGuiKey_Z] = System::KeyboardKeys::KeyZ;

    io.SetClipboardTextFn = SetClipboardTextCallback;
    io.GetClipboardTextFn = GetClipboardTextCallback;
    io.ClipboardUserData = m_engine->GetWindow().GetPrivateHandle();

    // Subscribe window event receivers.
    auto& windowEvents = m_engine->GetWindow().events;

    bool subscriptionResults = true;
    subscriptionResults |= windowEvents.cursorPosition.Subscribe(m_receiverCursorPosition, false, true);
    subscriptionResults |= windowEvents.mouseButton.Subscribe(m_receiverMouseButton, false, true);
    subscriptionResults |= windowEvents.mouseScroll.Subscribe(m_receiverMouseScroll, false, true);
    subscriptionResults |= windowEvents.keyboardKey.Subscribe(m_receiverKeyboardKey, false, true);
    subscriptionResults |= windowEvents.textInput.Subscribe(m_receiverTextInput, false, true);

    if(!subscriptionResults)
    {
        LOG_ERROR() << "Failed to subscribe to event receivers!";
        return false;
    }

    // Initialize the editor renderer.
    if(!m_editorRenderer.Initialize(engine))
    {
        LOG_ERROR() << "Could not initialize editor renderer!";
        return false;
    }

    // Initialize the game state editor.
    if(!m_editorGameState.Initialize())
    {
        LOG_ERROR() << "Could not initialize game state editor!";
        return false;
    }

    // Success!
    return m_initialized = true;
}

void EditorSystem::Update(float timeDelta)
{
    ASSERT(m_initialized, "Editor system has not been initialized!");

    // Set context as current.
    ImGui::SetCurrentContext(m_interface);
    ImGuiIO& io = ImGui::GetIO();

    // Set current delta time.
    io.DeltaTime = timeDelta;

    // Start a new interface frame.
    ImGui::NewFrame();

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
            if(ImGui::MenuItem("Show Demo"))
            {
                m_showDemoWindow = true;
            }

            ImGui::Separator();

            if(ImGui::MenuItem("Exit"))
            {
                m_engine->GetWindow().Close();
            }

            ImGui::EndMenu();
        }

        ImGui::Separator();

        ImGui::EndMainMenuBar();
    }

    // Update editor systems.
    m_editorGameState.Update(timeDelta);
}

void EditorSystem::Draw()
{
    ASSERT(m_initialized, "Editor system has not been initialized!");

    // Set the context and draw the editor interface.
    ImGui::SetCurrentContext(m_interface);
    m_editorRenderer.Draw();
}

void EditorSystem::CursorPositionCallback(const System::Window::Events::CursorPosition& event)
{
    ASSERT(m_initialized, "Editor system has not been initialized!");

    // Set context as current.
    ImGui::SetCurrentContext(m_interface);
    ImGuiIO& io = ImGui::GetIO();

    // Set cursor position.
    io.MousePos.x = (float)event.x;
    io.MousePos.y = (float)event.y;
}

bool EditorSystem::MouseButtonCallback(const System::Window::Events::MouseButton& event)
{
    ASSERT(m_initialized, "Editor system has not been initialized!");

    // Set context as current.
    ImGui::SetCurrentContext(m_interface);
    ImGuiIO& io = ImGui::GetIO();

    // Determine the number of supported mouse buttons.
    const std::size_t SupportedMouseButtonCount = std::min(
        Utility::StaticArraySize(io.MouseDown),
        (std::size_t)System::MouseButtons::Count
    );

    // We can only handle a specific number of buttons.
    if(event.button < System::MouseButtons::Button1)
        return false;

    if(event.button >= System::MouseButtons::Button1 + SupportedMouseButtonCount)
        return false;

    // Set mouse button state.
    const unsigned int MouseButtonIndex = event.button - System::MouseButtons::Button1;
    ASSERT(MouseButtonIndex < Utility::StaticArraySize(io.MouseDown), "Invalid mouse button index!");
    io.MouseDown[MouseButtonIndex] = (event.state == System::InputStates::Pressed);

    // Prevent input from passing through.
    return io.WantCaptureMouse;
}

bool EditorSystem::MouseScrollCallback(const System::Window::Events::MouseScroll& event)
{
    ASSERT(m_initialized, "Editor system has not been initialized!");

    // Set context as current.
    ImGui::SetCurrentContext(m_interface);
    ImGuiIO& io = ImGui::GetIO();

    // Set mouse wheel offset.
    io.MouseWheel = (float)event.offset;

    // Prevent input from passing through.
    return io.WantCaptureMouse;
}

bool EditorSystem::KeyboardKeyCallback(const System::Window::Events::KeyboardKey& event)
{
    ASSERT(m_initialized, "Editor system has not been initialized!");

    // Set context as current.
    ImGui::SetCurrentContext(m_interface);
    ImGuiIO& io = ImGui::GetIO();

    // Make sure that the array is of an expected size.
    const int MaxKeyboardKeyCount = Utility::StaticArraySize(io.KeysDown);
    ASSERT(MaxKeyboardKeyCount >= System::KeyboardKeys::Count, "Insufficient ImGUI keyboard state array size!");

    // We can only handle a specific number of keys.
    if(event.key < 0 || event.key >= MaxKeyboardKeyCount)
        return false;

    // Change key state.
    io.KeysDown[event.key] = (event.state == System::InputStates::Pressed);

    // Change states of key modifiers.
    io.KeyAlt = event.modifiers & System::KeyboardModifiers::Alt;
    io.KeyCtrl = event.modifiers & System::KeyboardModifiers::Ctrl;
    io.KeyShift = event.modifiers & System::KeyboardModifiers::Shift;
    io.KeySuper = event.modifiers & System::KeyboardModifiers::Super;

    // Prevent input from passing through.
    return io.WantCaptureKeyboard;
}

bool EditorSystem::TextInputCallback(const System::Window::Events::TextInput& event)
{
    ASSERT(m_initialized, "Editor system has not been initialized!");

    // Set context as current.
    ImGui::SetCurrentContext(m_interface);
    ImGuiIO& io = ImGui::GetIO();

    // Convert character from UTF-32 to UTF-8 encoding.
    // We will need an array for four UTF-8 characters and a null terminator.
    char utf8Character[5] = { 0 };

    ASSERT(utf8::internal::is_code_point_valid(event.utf32Character), "Invalid UTF-32 encoding!");
    utf8::unchecked::utf32to8(&event.utf32Character, &event.utf32Character + 1, &utf8Character[0]);

    // Add text input character.
    io.AddInputCharactersUTF8(&utf8Character[0]);

    // Prevent input from passing through.
    return io.WantCaptureKeyboard;
}

GameStateEditor& EditorSystem::GetGameStateEditor()
{
    return m_editorGameState;
}
