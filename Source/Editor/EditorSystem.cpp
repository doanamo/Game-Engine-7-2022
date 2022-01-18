/*
    Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Editor/EditorSystem.hpp"
#include "Editor/EditorRenderer.hpp"
#include "Editor/EditorConsole.hpp"
#include "Editor/EditorShell.hpp"
#include <Core/System/SystemStorage.hpp>
#include <Platform/TimerSystem.hpp>
#include <Platform/Timer.hpp>
#include <Platform/WindowSystem.hpp>
#include <Platform/Window.hpp>
#include <Platform/InputManager.hpp>
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
    m_timerSystem = &engineSystems.Locate<Platform::TimerSystem>();
    m_windowSystem = &engineSystems.Locate<Platform::WindowSystem>();

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
    ASSERT(m_windowSystem != nullptr);
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
    io.KeyMap[ImGuiKey_Tab] = Platform::KeyboardKeys::KeyTab;
    io.KeyMap[ImGuiKey_LeftArrow] = Platform::KeyboardKeys::KeyLeft;
    io.KeyMap[ImGuiKey_RightArrow] = Platform::KeyboardKeys::KeyRight;
    io.KeyMap[ImGuiKey_UpArrow] = Platform::KeyboardKeys::KeyUp;
    io.KeyMap[ImGuiKey_DownArrow] = Platform::KeyboardKeys::KeyDown;
    io.KeyMap[ImGuiKey_PageUp] = Platform::KeyboardKeys::KeyPageUp;
    io.KeyMap[ImGuiKey_PageDown] = Platform::KeyboardKeys::KeyPageDown;
    io.KeyMap[ImGuiKey_Home] = Platform::KeyboardKeys::KeyHome;
    io.KeyMap[ImGuiKey_End] = Platform::KeyboardKeys::KeyEnd;
    io.KeyMap[ImGuiKey_Insert] = Platform::KeyboardKeys::KeyInsert;
    io.KeyMap[ImGuiKey_Delete] = Platform::KeyboardKeys::KeyDelete;
    io.KeyMap[ImGuiKey_Backspace] = Platform::KeyboardKeys::KeyBackspace;
    io.KeyMap[ImGuiKey_Space] = Platform::KeyboardKeys::KeySpace;
    io.KeyMap[ImGuiKey_Enter] = Platform::KeyboardKeys::KeyEnter;
    io.KeyMap[ImGuiKey_Escape] = Platform::KeyboardKeys::KeyEscape;
    io.KeyMap[ImGuiKey_A] = Platform::KeyboardKeys::KeyA;
    io.KeyMap[ImGuiKey_C] = Platform::KeyboardKeys::KeyC;
    io.KeyMap[ImGuiKey_V] = Platform::KeyboardKeys::KeyV;
    io.KeyMap[ImGuiKey_X] = Platform::KeyboardKeys::KeyX;
    io.KeyMap[ImGuiKey_Y] = Platform::KeyboardKeys::KeyY;
    io.KeyMap[ImGuiKey_Z] = Platform::KeyboardKeys::KeyZ;
    io.SetClipboardTextFn = SetClipboardTextCallback;
    io.GetClipboardTextFn = GetClipboardTextCallback;
    io.ClipboardUserData = m_windowSystem->GetWindow().GetContext().GetPrivateHandle();

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

    if(!m_subsystems.Finalize())
    {
        LOG_ERROR(LogCreateSubsystemsFailed, "Could not finalize system storage.");
        return false;
    }

    return true;
}

bool EditorSystem::SubscribeEvents(const Core::EngineSystemStorage& engineSystems)
{
    auto& inputManager = engineSystems.Locate<Platform::InputManager>();

    Event::SubscriptionPolicy subscriptionPolicy = Event::SubscriptionPolicy::ReplaceSubscription;
    Event::PriorityPolicy priorityPolicy = Event::PriorityPolicy::InsertFront;

    bool subscriptionResults = true;

    Platform::InputState& inputState = inputManager.GetInputState();
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

void EditorSystem::OnBeginFrame()
{
    ImGui::SetCurrentContext(m_interface);

    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = m_timerSystem->GetTimer().GetDeltaSeconds();
    io.DisplaySize.x = (float)m_windowSystem->GetWindow().GetWidth();
    io.DisplaySize.y = (float)m_windowSystem->GetWindow().GetHeight();

    ImGui::NewFrame();

    m_subsystems.ForEach([timeDelta = io.DeltaTime](EditorSubsystem& subsystem)
    {
        subsystem.OnBeginInterface(timeDelta);
        return true;
    });
}

void EditorSystem::OnEndFrame()
{
    ImGui::SetCurrentContext(m_interface);
    ImGui::EndFrame();

    m_subsystems.ForEach([](EditorSubsystem& subsystem)
    {
        subsystem.OnEndInterface();
        return true;
    });
}

bool EditorSystem::OnTextInput(const Platform::InputEvents::TextInput& event)
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

bool EditorSystem::OnKeyboardKey(const Platform::InputEvents::KeyboardKey& event)
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
    ASSERT(MaxKeyboardKeyCount >= Platform::KeyboardKeys::Count, "Insufficient ImGUI keyboard state array size!");

    if(event.key < 0 || event.key >= MaxKeyboardKeyCount)
        return false;

    io.KeysDown[event.key] = event.state == Platform::InputStates::Pressed;
    io.KeyAlt = event.modifiers & Platform::KeyboardModifiers::Alt;
    io.KeyCtrl = event.modifiers & Platform::KeyboardModifiers::Ctrl;
    io.KeyShift = event.modifiers & Platform::KeyboardModifiers::Shift;
    io.KeySuper = event.modifiers & Platform::KeyboardModifiers::Super;
    return io.WantCaptureKeyboard;
}

bool EditorSystem::OnMouseButton(const Platform::InputEvents::MouseButton& event)
{
    ImGui::SetCurrentContext(m_interface);
    ImGuiIO& io = ImGui::GetIO();

    const std::size_t SupportedMouseButtonCount = std::min(
        Common::StaticArraySize(io.MouseDown),
        static_cast<std::size_t>(Platform::MouseButtons::Count));

    if(event.button < Platform::MouseButtons::Begin)
        return false;

    if(event.button >= Platform::MouseButtons::Begin + SupportedMouseButtonCount)
        return false;

    const unsigned int MouseButtonIndex = event.button - Platform::MouseButtons::Begin;
    io.MouseDown[MouseButtonIndex] = event.state == Platform::InputStates::Pressed;
    return io.WantCaptureMouse;
}

bool EditorSystem::OnMouseScroll(const Platform::InputEvents::MouseScroll& event)
{
    ImGui::SetCurrentContext(m_interface);
    ImGuiIO& io = ImGui::GetIO();

    io.MouseWheel = static_cast<float>(event.offset);

    return io.WantCaptureMouse;
}

void EditorSystem::OnCursorPosition(const Platform::InputEvents::CursorPosition& event)
{
    ImGui::SetCurrentContext(m_interface);
    ImGuiIO& io = ImGui::GetIO();

    io.MousePos.x = static_cast<float>(event.x);
    io.MousePos.y = static_cast<float>(event.y);
}
