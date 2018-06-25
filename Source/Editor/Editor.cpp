/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Editor/Editor.hpp"
using namespace Engine;

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

Editor::Editor() :
    m_context(nullptr),
    m_window(nullptr),
    m_initialized(false)
{
    // Bind event receivers.
    m_receiverCursorPosition.Bind<Editor, &Editor::CursorPositionCallback>(this);
    m_receiverMouseButton.Bind<Editor, &Editor::MouseButtonCallback>(this);
    m_receiverMouseScroll.Bind<Editor, &Editor::MouseScrollCallback>(this);
    m_receiverKeyboardKey.Bind<Editor, &Editor::KeyboardKeyCallback>(this);
    m_receiverTextInput.Bind<Editor, &Editor::TextInputCallback>(this);
}

Editor::~Editor()
{
    this->DestroyContext();
}

void Editor::DestroyContext()
{
    // Shutdown the rendering implementation.
    ImGui_ImplOpenGL3_Shutdown();

    // Destroy the user interface context.
    if(m_context)
    {
        ImGui::DestroyContext(m_context);
        m_context = nullptr;
    }
}

bool Editor::Initialize(System::Window* window)
{
    LOG() << "Initializing editor..." << LOG_INDENT();

    // Check if the instance is already initialized.
    VERIFY(!m_initialized, "Editor instance is already initialized!");

    // Check if the provided argument is valid.
    VERIFY(window != nullptr, "Invalid argument - \"window\" is nullptr!");

    // Create the user interface context.
    IMGUI_CHECKVERSION();
    
    m_context = ImGui::CreateContext();
    if(m_context == nullptr)
    {
        LOG_ERROR() << "Failed to initialize the user interface context!";
        return false;
    }

    SCOPE_GUARD_IF(!m_initialized, this->DestroyContext());

    // Setup user interface input.
    ImGuiIO& io = ImGui::GetIO();

    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
    io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
    io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
    io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
    io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
    io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
    io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
    io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
    io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
    io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
    io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

    io.SetClipboardTextFn = SetClipboardTextCallback;
    io.GetClipboardTextFn = GetClipboardTextCallback;
    io.ClipboardUserData = window->GetPrivateHandle();

    // Subscribe window event receivers.
    SCOPE_GUARD_BEGIN(!m_initialized);
    {
        m_receiverCursorPosition.Unsubscribe();
        m_receiverMouseButton.Unsubscribe();
        m_receiverMouseScroll.Unsubscribe();
        m_receiverKeyboardKey.Unsubscribe();
        m_receiverTextInput.Unsubscribe();
    }
    SCOPE_GUARD_END();

    bool subscriptionResults = true;
    subscriptionResults |= window->events.cursorPosition.Subscribe(m_receiverCursorPosition);
    subscriptionResults |= window->events.mouseButton.Subscribe(m_receiverMouseButton);
    subscriptionResults |= window->events.mouseScroll.Subscribe(m_receiverMouseScroll);
    subscriptionResults |= window->events.keyboardKey.Subscribe(m_receiverKeyboardKey);
    subscriptionResults |= window->events.textInput.Subscribe(m_receiverTextInput);

    if(!subscriptionResults)
    {
        LOG_ERROR() << "Failed to subscribe event receivers!";
        return false;
    }

    // Initialize the rendering implementation.
    ImGui_ImplOpenGL3_Init();

    // Save window reference.
    m_window = window;

    // Success!
    LOG_INFO() << "Success!";

    return m_initialized = true;
}

void Editor::Update(float dt)
{
    // Set context as current.
    ImGui::SetCurrentContext(m_context);
    ImGuiIO& io = ImGui::GetIO();

    // Set current delta time.
    io.DeltaTime = dt;

    // Set current display size.
    io.DisplaySize.x = (float)m_window->GetWidth();
    io.DisplaySize.y = (float)m_window->GetHeight();

    // Begin a new rendering frame.
    ImGui_ImplOpenGL3_NewFrame();

    // Start a new interface frame.
    ImGui::NewFrame();

    // Show a demo window.
    bool showDemoWindow = true;
    ImGui::ShowDemoWindow(&showDemoWindow);
}

void Editor::Draw()
{
    // Set context as current.
    ImGui::SetCurrentContext(m_context);

    // End our interface frame.
    ImGui::EndFrame();
    ImGui::Render();

    // Draw our rendered interface frame.
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Editor::CursorPositionCallback(const System::Window::Events::CursorPosition& event)
{
    // Set context as current.
    ImGui::SetCurrentContext(m_context);
    ImGuiIO& io = ImGui::GetIO();

    // Set cursor position.
    io.MousePos.x = (float)event.x;
    io.MousePos.y = (float)event.y;
}

void Editor::MouseButtonCallback(const System::Window::Events::MouseButton& event)
{
    // Set context as current.
    ImGui::SetCurrentContext(m_context);
    ImGuiIO& io = ImGui::GetIO();

    // Make sure that the array is of an expected size.
    const int MaxButtonCount = 5;
    ASSERT(Utility::StaticArraySize(io.MouseDown) == MaxButtonCount, "Unexpected array size!");

    // We can only handle a specific number of buttons.
    if(event.button < 0 || event.button >= MaxButtonCount)
        return;

    // Set mouse button state.
    io.MouseDown[event.button] = event.action == GLFW_PRESS;
}

void Editor::MouseScrollCallback(const System::Window::Events::MouseScroll& event)
{
    // Set context as current.
    ImGui::SetCurrentContext(m_context);
    ImGuiIO& io = ImGui::GetIO();

    // Set mouse wheel offset.
    io.MouseWheel = (float)event.offset;
}

void Editor::KeyboardKeyCallback(const System::Window::Events::KeyboardKey& event)
{
    // Set context as current.
    ImGui::SetCurrentContext(m_context);
    ImGuiIO& io = ImGui::GetIO();

    // Make sure that the array is of an expected size.
    const int MaxKeyCount = 512;
    ASSERT(Utility::StaticArraySize(io.KeysDown) == MaxKeyCount, "Unexpected array size!");

    // We can only handle a specific number of keys.
    if(event.key < 0 || event.key >= MaxKeyCount)
        return;

    // Change key state.
    io.KeysDown[event.key] = event.action == GLFW_PRESS;

    // Change states of key modifiers.
    io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
    io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
    io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
    io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
}

void Editor::TextInputCallback(const System::Window::Events::TextInput& event)
{
    // Set context as current.
    ImGui::SetCurrentContext(m_context);
    ImGuiIO& io = ImGui::GetIO();

    // Add text input character.
    io.AddInputCharacter(event.character);
}
