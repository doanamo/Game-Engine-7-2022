/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Editor/EditorSystem.hpp"
#include "System/ResourceManager.hpp"
#include "Engine.hpp"
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
    // Call the move assignment.
    *this = std::move(other);
}

EditorSystem& EditorSystem::operator=(EditorSystem&& other)
{
    // Swap class members.
    std::swap(m_engine, other.m_engine);
    std::swap(m_interface, other.m_interface);
    std::swap(m_showDemoWindow, other.m_showDemoWindow);

    std::swap(m_receiverCursorPosition, other.m_receiverCursorPosition);
    std::swap(m_receiverMouseButton, other.m_receiverMouseButton);
    std::swap(m_receiverMouseScroll, other.m_receiverMouseScroll);
    std::swap(m_receiverKeyboardKey, other.m_receiverKeyboardKey);
    std::swap(m_receiverTextInput, other.m_receiverTextInput);

    std::swap(m_vertexBuffer, other.m_vertexBuffer);
    std::swap(m_indexBuffer, other.m_indexBuffer);
    std::swap(m_vertexArray, other.m_vertexArray);
    std::swap(m_fontTexture, other.m_fontTexture);
    std::swap(m_sampler, other.m_sampler);
    std::swap(m_shader, other.m_shader);

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

    // Validate arguments.
    if(engine == nullptr)
    {
        LOG_ERROR() << "Invalid argument - \"engine\" is invalid!";
        return false;
    }

    // Save engine reference.
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
    io.ClipboardUserData = m_engine->window.GetPrivateHandle();

    // Subscribe window event receivers.
    bool subscriptionResults = true;
    subscriptionResults |= m_engine->window.events.cursorPosition.Subscribe(m_receiverCursorPosition);
    subscriptionResults |= m_engine->window.events.mouseButton.Subscribe(m_receiverMouseButton);
    subscriptionResults |= m_engine->window.events.mouseScroll.Subscribe(m_receiverMouseScroll);
    subscriptionResults |= m_engine->window.events.keyboardKey.Subscribe(m_receiverKeyboardKey);
    subscriptionResults |= m_engine->window.events.textInput.Subscribe(m_receiverTextInput);

    if(!subscriptionResults)
    {
        LOG_ERROR() << "Failed to subscribe to event receivers!";
        return false;
    }

    // Create a vertex buffer.
    Graphics::BufferInfo vertexBufferInfo;
    vertexBufferInfo.usage = GL_STREAM_DRAW;
    vertexBufferInfo.elementSize = sizeof(ImDrawVert);

    if(!m_vertexBuffer.Initialize(&m_engine->renderContext, vertexBufferInfo))
    {
        LOG_ERROR() << "Could not initialize vertex buffer!";
        return false;
    }

    // Create an index buffer.
    Graphics::BufferInfo indexBufferInfo;
    indexBufferInfo.usage = GL_STREAM_DRAW;
    indexBufferInfo.elementSize = sizeof(ImDrawIdx);

    if(!m_indexBuffer.Initialize(&m_engine->renderContext, indexBufferInfo))
    {
        LOG_ERROR() << "Could not initialize index buffer!";
        return false;
    }

    // Create an input layout.
    const Graphics::VertexAttribute inputAttributes[] =
    {
        { &m_vertexBuffer, Graphics::VertexAttributeType::Vector2, GL_FLOAT,         false }, // Position
        { &m_vertexBuffer, Graphics::VertexAttributeType::Vector2, GL_FLOAT,         false }, // Texture
        { &m_vertexBuffer, Graphics::VertexAttributeType::Vector4, GL_UNSIGNED_BYTE, true  }, // Color
    };

    Graphics::VertexArrayInfo inputLayoutInfo;
    inputLayoutInfo.attributeCount = Utility::StaticArraySize(inputAttributes);
    inputLayoutInfo.attributes = &inputAttributes[0];

    if(!m_vertexArray.Initialize(&m_engine->renderContext, inputLayoutInfo))
    {
        LOG_ERROR() << "Could not initialize vertex array!";
        return false;
    }

    // Retrieve built in font data.
    unsigned char* fontData = nullptr;
    int fontWidth = 0;
    int fontHeight = 0;

    ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&fontData, &fontWidth, &fontHeight);
    
    if(fontData == nullptr || fontWidth == 0 || fontHeight == 0)
    {
        LOG_ERROR() << "Could not retrieve font data!";
        return false;
    }

    // Create a font texture.
    Graphics::Texture::CreateFromParams textureParams;
    textureParams.engine = m_engine;
    textureParams.width = fontWidth;
    textureParams.height = fontHeight;
    textureParams.format = GL_RGBA;
    textureParams.mipmaps = false;
    textureParams.data = fontData;

    if(!m_fontTexture.Initialize(textureParams))
    {
        LOG_ERROR() << "Could not initialize font texture!";
        return false;
    }

    ImGui::GetIO().Fonts->TexID = (void*)(intptr_t)m_fontTexture.GetHandle();

    // Create a sampler.
    // Set linear filtering otherwise textures without mipmaps will be black.
    Graphics::SamplerInfo samplerInfo;
    samplerInfo.textureMinFilter = GL_LINEAR;
    samplerInfo.textureMagFilter = GL_LINEAR;

    if(!m_sampler.Initialize(&m_engine->renderContext, samplerInfo))
    {
        LOG_ERROR() << "Could not initialize sampler!";
        return false;
    }

    // Load a shader.
    Graphics::Shader::LoadFromFile shaderParams;
    shaderParams.engine = m_engine;
    shaderParams.filePath = "Data/Engine/Shaders/Interface.shader";

    m_shader = m_engine->resourceManager.Acquire<Graphics::Shader>(
        shaderParams.filePath, shaderParams);

    if(m_shader == nullptr)
    {
        LOG_ERROR() << "Could not initialize shader!";
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

    // Set current display size.
    io.DisplaySize.x = (float)m_engine->window.GetWidth();
    io.DisplaySize.y = (float)m_engine->window.GetHeight();

    // Start a new interface frame.
    ImGui::NewFrame();

    // Show demo window.
    if(m_showDemoWindow)
    {
        ImGui::ShowDemoWindow(&m_showDemoWindow);
    }

    // Display standard editor interface if no custom one is used.
    if(ImGui::BeginMainMenuBar())
    {
        if(ImGui::BeginMenu("Menu"))
        {
            if(ImGui::MenuItem("Show demo window"))
            {
                m_showDemoWindow = true;
            }

            ImGui::Separator();

            if(ImGui::MenuItem("Exit"))
            {
                m_engine->window.Close();
            }

            ImGui::EndMenu();
        }

        ImGui::Separator();

        ImGui::EndMainMenuBar();
    }
}

void EditorSystem::Draw()
{
    ASSERT(m_initialized, "Editor system has not been initialized!");

    // Set context as current.
    ImGui::SetCurrentContext(m_interface);
    ImGuiIO& io = ImGui::GetIO();

    // End our rendering frame.
    ImGui::EndFrame();
    ImGui::Render();

    // Get interface render data.
    ImDrawData* drawData = ImGui::GetDrawData();

    // Calculate rendering transform.
    glm::mat4 transform = glm::ortho(
        0.0f, (float)m_engine->window.GetWidth(), 
        (float)m_engine->window.GetHeight(), 0.0f
    );

    // Push a rendering state.
    auto& renderState = m_engine->renderContext.PushState();
    SCOPE_GUARD(m_engine->renderContext.PopState());

    // Prepare rendering state.
    renderState.Enable(GL_BLEND);
    renderState.BlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    renderState.BlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    renderState.Enable(GL_SCISSOR_TEST);

    renderState.Viewport(0, 0, m_engine->window.GetWidth(), m_engine->window.GetHeight());

    renderState.UseProgram(m_shader->GetHandle());
    m_shader->SetUniform("vertexTransform", transform);
    m_shader->SetUniform("textureDiffuse", 0);

    renderState.BindSampler(0, m_sampler.GetHandle());

    // Process draw data.
    ImVec2 position = drawData->DisplayPos;
    for(int list = 0; list < drawData->CmdListsCount; ++list)
    {
        const ImDrawList* commandList = drawData->CmdLists[list];
        const ImDrawIdx* indexBufferOffset = 0;

        m_vertexBuffer.Update(commandList->VtxBuffer.Data, commandList->VtxBuffer.Size);
        m_indexBuffer.Update(commandList->IdxBuffer.Data, commandList->IdxBuffer.Size);

        for(int command = 0; command < commandList->CmdBuffer.Size; ++command)
        {
            const ImDrawCmd* drawCommand = &commandList->CmdBuffer[command];

            if(drawCommand->UserCallback)
            {
                drawCommand->UserCallback(commandList, drawCommand);
            }
            else
            {
                ImVec4 clipRect;
                clipRect.x = drawCommand->ClipRect.x - position.x;
                clipRect.y = drawCommand->ClipRect.y - position.y;
                clipRect.z = drawCommand->ClipRect.z - position.x;
                clipRect.w = drawCommand->ClipRect.w - position.y;

                renderState.Scissor(
                    (int)clipRect.x,
                    (int)(m_engine->window.GetHeight() - clipRect.w),
                    (int)(clipRect.z - clipRect.x),
                    (int)(clipRect.w - clipRect.y)
                );

                renderState.ActiveTexture(GL_TEXTURE0);
                renderState.BindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)drawCommand->TextureId);

                renderState.BindVertexArray(m_vertexArray.GetHandle());
                renderState.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.GetHandle());
                renderState.DrawElements(GL_TRIANGLES, (GLsizei)drawCommand->ElemCount, GL_UNSIGNED_SHORT, indexBufferOffset);
            }

            indexBufferOffset += drawCommand->ElemCount;
        }
    }
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

void EditorSystem::MouseButtonCallback(const System::Window::Events::MouseButton& event)
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
        return;

    if(event.button >= System::MouseButtons::Button1 + SupportedMouseButtonCount)
        return;

    // Set mouse button state.
    const unsigned int MouseButtonIndex = event.button - System::MouseButtons::Button1;
    ASSERT(MouseButtonIndex < Utility::StaticArraySize(io.MouseDown), "Invalid mouse button index!");
    io.MouseDown[MouseButtonIndex] = (event.state == System::InputStates::Pressed);
}

void EditorSystem::MouseScrollCallback(const System::Window::Events::MouseScroll& event)
{
    ASSERT(m_initialized, "Editor system has not been initialized!");

    // Set context as current.
    ImGui::SetCurrentContext(m_interface);
    ImGuiIO& io = ImGui::GetIO();

    // Set mouse wheel offset.
    io.MouseWheel = (float)event.offset;
}

void EditorSystem::KeyboardKeyCallback(const System::Window::Events::KeyboardKey& event)
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
        return;

    // Change key state.
    io.KeysDown[event.key] = (event.state == System::InputStates::Pressed);

    // Change states of key modifiers.
    io.KeyAlt = event.modifiers & System::KeyboardModifiers::ModAlt;
    io.KeyCtrl = event.modifiers & System::KeyboardModifiers::ModCtrl;
    io.KeyShift = event.modifiers & System::KeyboardModifiers::ModShift;
    io.KeySuper = event.modifiers & System::KeyboardModifiers::ModSuper;
}

void EditorSystem::TextInputCallback(const System::Window::Events::TextInput& event)
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
}
