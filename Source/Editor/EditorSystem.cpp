/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Editor/Precompiled.hpp"
#include "Editor/EditorSystem.hpp"
#include "Editor/EditorRenderer.hpp"
#include "Editor/EditorConsole.hpp"
#include "Editor/EditorShell.hpp"
#include <Core/SystemStorage.hpp>
#include <System/Window.hpp>
#include <System/InputManager.hpp>
#include <Game/GameInstance.hpp>
using namespace Editor;

namespace
{
    const char* LogAttachFailed = "Failed to attach editor system! {}";
    const char* LogCreateSubsystemsFailed = "Failed to create editor subsystems! {}";
    const char* LogSubscribeEventsFailed = "Failed to subscribe editor events! {}";

    void SetClipboardTextCallback(void* userData, const char* text)
    {
        ASSERT(userData != nullptr, "User data argument is nullptr!");
        glfwSetClipboardString((GLFWwindow*)userData, text);
    }

    const char* GetClipboardTextCallback(void* userData)
    {
        ASSERT(userData != nullptr, "User data argument is nullptr!");
        return glfwGetClipboardString((GLFWwindow*)userData);
    }
}

EditorSystem::EditorSystem()
{
    m_receiverTextInput.Bind<EditorSystem, &EditorSystem::OnTextInput>(this);
    m_receiverKeyboardKey.Bind<EditorSystem, &EditorSystem::OnKeyboardKey>(this);
    m_receiverMouseButton.Bind<EditorSystem, &EditorSystem::OnMouseButton>(this);
    m_receiverMouseScroll.Bind<EditorSystem, &EditorSystem::OnMouseScroll>(this);
    m_receiverCursorPosition.Bind<EditorSystem, &EditorSystem::OnCursorPosition>(this);
}

EditorSystem::~EditorSystem()
{
    if(m_interface)
    {
        ImGui::DestroyContext(m_interface);
        m_interface = nullptr;
    }
}

bool EditorSystem::OnAttach(const Core::EngineSystemStorage& engineSystems)
{
    // Acquire engine systems.
    m_window = engineSystems.Locate<System::Window>();
    if(!m_window)
    {
        LOG_ERROR(LogAttachFailed, "Could not locate window.");
        return false;
    }

    // Initialize editor system.
    if(!CreateContext())
    {
        LOG_ERROR(LogAttachFailed, "Could not create editor context.");
        return false;
    }

    if(!CreateSubsystems(engineSystems))
    {
        LOG_ERROR(LogAttachFailed, "Could not create editor subsystems.");
        return false;
    }

    if(!SubscribeEvents(engineSystems))
    {
        LOG_ERROR(LogAttachFailed, "Could not subscribe editor events.");
        return false;
    }

    return true;
}

bool EditorSystem::CreateContext()
{
    ASSERT(m_window != nullptr);
    ASSERT(m_interface == nullptr);

    // Create ImGui context.
    m_interface = ImGui::CreateContext();
    if(m_interface == nullptr)
        return false;

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
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
    io.ClipboardUserData = m_window->GetContext().GetPrivateHandle();

    return true;
}

bool EditorSystem::CreateSubsystems(const Core::EngineSystemStorage& engineSystems)
{
    // Create editor subsystem context.
    auto context = std::make_unique<EditorSubsystemContext>(engineSystems);
    if(!context || !m_subsystems.Attach(std::move(context)))
    {
        LOG_ERROR(LogCreateSubsystemsFailed, "Could not create engine subsystem context.");
        return false;
    }

    // Create editor subsystems.
    const std::vector<Reflection::TypeIdentifier> defaultEditorSubsystemTypes =
    {
        Reflection::GetIdentifier<EditorShell>(),
        Reflection::GetIdentifier<EditorConsole>(),
        Reflection::GetIdentifier<EditorRenderer>(),
    };

    if(!m_subsystems.CreateFromTypes(defaultEditorSubsystemTypes))
    {
        LOG_ERROR(LogCreateSubsystemsFailed, "Could not populate system storage.");
        return false;
    }

    return true;
}

bool EditorSystem::SubscribeEvents(const Core::EngineSystemStorage& engineSystems)
{
    auto* inputManager = engineSystems.Locate<System::InputManager>();
    if(!inputManager)
    {
        LOG_ERROR(LogSubscribeEventsFailed, "Could not locate input manager.");
        return false;
    }

    Event::SubscriptionPolicy subscriptionPolicy = Event::SubscriptionPolicy::ReplaceSubscription;
    Event::PriorityPolicy priorityPolicy = Event::PriorityPolicy::InsertFront;

    bool subscriptionResults = true;

    System::InputState& inputState = inputManager->GetInputState();
    subscriptionResults &= inputState.events.Subscribe(
        m_receiverKeyboardKey, subscriptionPolicy, priorityPolicy);
    subscriptionResults &= inputState.events.Subscribe(
        m_receiverTextInput, subscriptionPolicy, priorityPolicy);
    subscriptionResults &= inputState.events.Subscribe(
        m_receiverMouseButton, subscriptionPolicy, priorityPolicy);
    subscriptionResults &= inputState.events.Subscribe(
        m_receiverMouseScroll, subscriptionPolicy, priorityPolicy);
    subscriptionResults &= inputState.events.Subscribe(
        m_receiverCursorPosition, subscriptionPolicy, priorityPolicy);

    return subscriptionResults;
}

void EditorSystem::BeginInterface(float timeDelta)
{
    ImGui::SetCurrentContext(m_interface);

    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = timeDelta;
    io.DisplaySize.x = (float)m_window->GetWidth();
    io.DisplaySize.y = (float)m_window->GetHeight();

    ImGui::NewFrame();

    m_subsystems.ForEach([timeDelta](EditorSubsystem& subsystem)
    {
        subsystem.OnBeginInterface(timeDelta);
        return true;
    });
}

void EditorSystem::EndInterface()
{
    ImGui::SetCurrentContext(m_interface);
    ImGui::EndFrame();

    m_subsystems.ForEach([](EditorSubsystem& subsystem)
    {
        subsystem.OnEndInterface();
        return true;
    });
}

bool EditorSystem::OnTextInput(const System::InputEvents::TextInput& event)
{
    /*
        Convert character from UTF-32 to UTF-8 encoding.
        Needs temporary array for four UTF-8 code points (octets).
        Extra character for null terminator is included.
    */

    ImGui::SetCurrentContext(m_interface);
    ImGuiIO& io = ImGui::GetIO();

    char utf8Character[5] = { 0 };
    ASSERT(utf8::internal::is_code_point_valid(event.utf32Character), "Invalid UTF-32 encoding!");
    utf8::unchecked::utf32to8(&event.utf32Character, &event.utf32Character + 1, &utf8Character[0]);

    io.AddInputCharactersUTF8(&utf8Character[0]);
    return io.WantCaptureKeyboard;
}

bool EditorSystem::OnKeyboardKey(const System::InputEvents::KeyboardKey& event)
{
    ImGui::SetCurrentContext(m_interface);
    ImGuiIO& io = ImGui::GetIO();

    m_subsystems.ForEach([&io, &event](EditorSubsystem& subsystem)
    {
        if(subsystem.OnKeyboardKey(event))
        {
            io.WantCaptureKeyboard = true;
            return false;
        }

        return true;
    });

    const size_t MaxKeyboardKeyCount = Common::StaticArraySize(io.KeysDown);
    ASSERT(MaxKeyboardKeyCount >= System::KeyboardKeys::Count, "Insufficient ImGUI keyboard state array size!");

    if(event.key < 0 || event.key >= MaxKeyboardKeyCount)
        return false;

    io.KeysDown[event.key] = event.state == System::InputStates::Pressed;
    io.KeyAlt = event.modifiers & System::KeyboardModifiers::Alt;
    io.KeyCtrl = event.modifiers & System::KeyboardModifiers::Ctrl;
    io.KeyShift = event.modifiers & System::KeyboardModifiers::Shift;
    io.KeySuper = event.modifiers & System::KeyboardModifiers::Super;
    return io.WantCaptureKeyboard;
}

bool EditorSystem::OnMouseButton(const System::InputEvents::MouseButton& event)
{
    ImGui::SetCurrentContext(m_interface);
    ImGuiIO& io = ImGui::GetIO();

    const std::size_t SupportedMouseButtonCount = std::min(
        Common::StaticArraySize(io.MouseDown),
        static_cast<std::size_t>(System::MouseButtons::Count));

    if(event.button < System::MouseButtons::Begin)
        return false;

    if(event.button >= System::MouseButtons::Begin + SupportedMouseButtonCount)
        return false;

    const unsigned int MouseButtonIndex = event.button - System::MouseButtons::Begin;
    io.MouseDown[MouseButtonIndex] = event.state == System::InputStates::Pressed;
    return io.WantCaptureMouse;
}

bool EditorSystem::OnMouseScroll(const System::InputEvents::MouseScroll& event)
{
    ImGui::SetCurrentContext(m_interface);
    ImGuiIO& io = ImGui::GetIO();

    io.MouseWheel = static_cast<float>(event.offset);

    return io.WantCaptureMouse;
}

void EditorSystem::OnCursorPosition(const System::InputEvents::CursorPosition& event)
{
    ImGui::SetCurrentContext(m_interface);
    ImGuiIO& io = ImGui::GetIO();

    io.MousePos.x = static_cast<float>(event.x);
    io.MousePos.y = static_cast<float>(event.y);
}
