/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Editor/EditorSystem.hpp"
#include <System/InputManager.hpp>
#include <Game/GameState.hpp>
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

EditorSystem::EditorSystem()
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
    if(m_interface)
    {
        ImGui::DestroyContext(m_interface);
        m_interface = nullptr;
    }
}

EditorSystem::CreateResult EditorSystem::Create(const CreateFromParams& params)
{
    LOG("Creating editor system...");
    LOG_SCOPED_INDENT();

    // Validate arguments.
    CHECK_ARGUMENT_OR_RETURN(params.fileSystem != nullptr, Common::Failure(CreateErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(params.resourceManager != nullptr, Common::Failure(CreateErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(params.inputManager != nullptr, Common::Failure(CreateErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(params.window != nullptr, Common::Failure(CreateErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(params.renderContext != nullptr, Common::Failure(CreateErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(params.gameFramework != nullptr, Common::Failure(CreateErrors::InvalidArgument));

    // Create instance.
    auto instance = std::unique_ptr<EditorSystem>(new EditorSystem());

    // Create ImGui context.
    instance->m_interface = ImGui::CreateContext();

    if(instance->m_interface == nullptr)
    {
        LOG_ERROR("Failed to initialize user interface context!");
        return Common::Failure(CreateErrors::FailedContextCreation);
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
    io.ClipboardUserData = params.window->GetPrivateHandle();

    // Subscribe input event receivers.
    // We insert receivers in front of dispatcher queue
    // as we want to have priority for input events.
    bool subscriptionResult = true;
    subscriptionResult &= params.inputManager->events.keyboardKey.Subscribe(instance->m_receiverKeyboardKey, false, true);
    subscriptionResult &= params.inputManager->events.textInput.Subscribe(instance->m_receiverTextInput, false, true);
    subscriptionResult &= params.inputManager->events.mouseButton.Subscribe(instance->m_receiverMouseButton, false, true);
    subscriptionResult &= params.inputManager->events.mouseScroll.Subscribe(instance->m_receiverMouseScroll, false, true);
    subscriptionResult &= params.inputManager->events.cursorPosition.Subscribe(instance->m_receiverCursorPosition, false, true);

    if(!subscriptionResult)
    {
        LOG_ERROR("Failed to subscribe to event receivers!");
        return Common::Failure(CreateErrors::FailedEventSubscription);
    }

    // Create editor renderer.
    EditorRenderer::CreateFromParams editorRendererParams;
    editorRendererParams.fileSystem = params.fileSystem;
    editorRendererParams.resourceManager = params.resourceManager;
    editorRendererParams.window = params.window;
    editorRendererParams.renderContext = params.renderContext;

    instance->m_editorRenderer = EditorRenderer::Create(editorRendererParams).UnwrapOr(nullptr);
    if(instance->m_editorRenderer == nullptr)
    {
        LOG_ERROR("Could not create editor renderer!");
        return Common::Failure(CreateErrors::FailedSubsystemCreation);
    }

    // Create editor shell.
    EditorShell::CreateFromParams editorShellParams;
    editorShellParams.window = params.window;
    editorShellParams.gameFramework = params.gameFramework;

    instance->m_editorShell = EditorShell::Create(editorShellParams).UnwrapOr(nullptr);
    if(instance->m_editorShell == nullptr)
    {
        LOG_ERROR("Could not create editor shell!");
        return Common::Failure(CreateErrors::FailedSubsystemCreation);
    }

    // Success!
    return Common::Success(std::move(instance));
}

void EditorSystem::Update(float timeDelta)
{
    // Set context as current.
    ImGui::SetCurrentContext(m_interface);
    ImGuiIO& io = ImGui::GetIO();

    // Set current delta time.
    io.DeltaTime = timeDelta;

    // Start new interface frame.
    ImGui::NewFrame();

    // Update editor shell.
    m_editorShell->Update(timeDelta);
}

void EditorSystem::Draw()
{
    // Set context and draw the editor interface.
    ImGui::SetCurrentContext(m_interface);
    m_editorRenderer->Draw();
}

void EditorSystem::CursorPositionCallback(const System::InputEvents::CursorPosition& event)
{
    // Set context as current.
    ImGui::SetCurrentContext(m_interface);
    ImGuiIO& io = ImGui::GetIO();

    // Set cursor position.
    io.MousePos.x = (float)event.x;
    io.MousePos.y = (float)event.y;
}

bool EditorSystem::MouseButtonCallback(const System::InputEvents::MouseButton& event)
{
    // Set context as current.
    ImGui::SetCurrentContext(m_interface);
    ImGuiIO& io = ImGui::GetIO();

    // Determine number of supported mouse buttons.
    const std::size_t SupportedMouseButtonCount = std::min(
        Common::StaticArraySize(io.MouseDown),
        (std::size_t)System::MouseButtons::Count
    );

    // We can only handle specific number of mouse buttons.
    if(event.button < System::MouseButtons::Button1)
        return false;

    if(event.button >= System::MouseButtons::Button1 + SupportedMouseButtonCount)
        return false;

    // Set mouse button state.
    const unsigned int MouseButtonIndex = event.button - System::MouseButtons::Button1;
    io.MouseDown[MouseButtonIndex] = (event.state == System::InputStates::Pressed);

    // Prevent input from passing through.
    return io.WantCaptureMouse;
}

bool EditorSystem::MouseScrollCallback(const System::InputEvents::MouseScroll& event)
{
    // Set context as current.
    ImGui::SetCurrentContext(m_interface);
    ImGuiIO& io = ImGui::GetIO();

    // Set mouse wheel offset.
    io.MouseWheel = (float)event.offset;

    // Prevent input from passing through.
    return io.WantCaptureMouse;
}

bool EditorSystem::KeyboardKeyCallback(const System::InputEvents::KeyboardKey& event)
{
    // Set context as current.
    ImGui::SetCurrentContext(m_interface);
    ImGuiIO& io = ImGui::GetIO();

    // Make sure that the array is of an expected size.
    const size_t MaxKeyboardKeyCount = Common::StaticArraySize(io.KeysDown);
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

bool EditorSystem::TextInputCallback(const System::InputEvents::TextInput& event)
{
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
