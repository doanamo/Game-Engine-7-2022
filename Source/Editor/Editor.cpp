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

Editor::Editor(Graphics::RenderContext* graphics) :
    m_graphics(graphics),
    m_vertexBuffer(graphics),
    m_indexBuffer(graphics),
    m_vertexArray(graphics),
    m_fontTexture(graphics),
    m_shader(graphics),
    m_interface(nullptr),
    m_window(nullptr),
    m_initialized(false)
{
    VERIFY(graphics && graphics->IsValid(), "Graphics context is invalid!");

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
    // Destroy the user interface context.
    if(m_interface)
    {
        ImGui::DestroyContext(m_interface);
        m_interface = nullptr;
    }
}

bool Editor::Initialize(System::Window* window)
{
    LOG() << "Initializing editor..." << LOG_INDENT();

    // Check if the instance is already initialized.
    VERIFY(!m_initialized, "Editor instance is already initialized!");

    // Check if the provided arguments are valid.
    VERIFY(window && window->IsValid(), "Invalid argument - \"window\" is invalid!");

    // Create the user interface context.
    m_interface = ImGui::CreateContext();

    if(m_interface == nullptr)
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

    // Create a vertex buffer.
    Graphics::BufferInfo vertexBufferInfo;
    vertexBufferInfo.usage = GL_STREAM_DRAW;
    vertexBufferInfo.elementSize = sizeof(ImDrawVert);

    if(!m_vertexBuffer.Create(vertexBufferInfo))
        return false;

    SCOPE_GUARD_IF(!m_initialized, m_vertexBuffer = Graphics::VertexBuffer(m_graphics));

    // Create an index buffer.
    Graphics::BufferInfo indexBufferInfo;
    indexBufferInfo.usage = GL_STREAM_DRAW;
    indexBufferInfo.elementSize = sizeof(ImDrawIdx);

    if(!m_indexBuffer.Create(indexBufferInfo))
        return false;

    SCOPE_GUARD_IF(!m_initialized, m_indexBuffer = Graphics::IndexBuffer(m_graphics));

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

    if(!m_vertexArray.Create(inputLayoutInfo))
        return false;

    SCOPE_GUARD_IF(!m_initialized, m_vertexArray = Graphics::VertexArray(m_graphics));

    // Create a font texture.
    unsigned char* fontData = nullptr;
    int fontWidth = 0;
    int fontHeight = 0;

    io.Fonts->GetTexDataAsRGBA32(&fontData, &fontWidth, &fontHeight);
    
    if(fontData == nullptr || fontWidth == 0 || fontHeight == 0)
    {
        LOG_ERROR() << "Could not retrieve font data!";
        return false;
    }

    if(!m_fontTexture.Create(fontWidth, fontHeight, GL_RGBA, (void*)fontData))
        return false;

    SCOPE_GUARD_IF(!m_initialized, m_fontTexture = Graphics::Texture(m_graphics));

    io.Fonts->TexID = (void *)(intptr_t)m_fontTexture.GetHandle();

    // Load a shader.
    if(!m_shader.Load(Build::GetWorkingDir() + "Data/Shaders/Interface.shader"))
        return false;

    SCOPE_GUARD_IF(!m_initialized, m_shader = Graphics::Shader(m_graphics));

    // Save window reference.
    m_window = window;

    // Success!
    LOG_INFO() << "Success!";

    return m_initialized = true;
}

void Editor::Update(float deltaTime)
{
    // Set context as current.
    ImGui::SetCurrentContext(m_interface);
    ImGuiIO& io = ImGui::GetIO();

    // Set current delta time.
    io.DeltaTime = deltaTime;

    // Set current display size.
    io.DisplaySize.x = (float)m_window->GetWidth();
    io.DisplaySize.y = (float)m_window->GetHeight();

    // Start a new interface frame.
    ImGui::NewFrame();

    // Show a demo window.
    bool showDemoWindow = true;
    ImGui::ShowDemoWindow(&showDemoWindow);
}

void Editor::Draw()
{
    // Set context as current.
    ImGui::SetCurrentContext(m_interface);
    ImGuiIO& io = ImGui::GetIO();

    // End our interface frame.
    ImGui::EndFrame();
    ImGui::Render();

    ImDrawData* drawData = ImGui::GetDrawData();

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    glViewport(0, 0, m_window->GetWidth(), m_window->GetHeight());
    glm::mat4 transform = glm::ortho(0.0f, (float)m_window->GetWidth(), (float)m_window->GetHeight(), 0.0f);

    glUseProgram(m_shader.GetHandle());
    glUniformMatrix4fv(m_shader.GetUniformIndex("vertexTransform"), 1, GL_FALSE, glm::value_ptr(transform));
    glUniform1i(m_shader.GetUniformIndex("textureDiffuse"), 0);

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

                glScissor(
                    (int)clipRect.x,
                    (int)(m_window->GetHeight() - clipRect.w),
                    (int)(clipRect.z - clipRect.x),
                    (int)(clipRect.w - clipRect.y)
                );

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)drawCommand->TextureId);

                glBindVertexArray(m_vertexArray.GetHandle());
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.GetHandle());
                glDrawElements(GL_TRIANGLES, (GLsizei)drawCommand->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, indexBufferOffset);
            }

            indexBufferOffset += drawCommand->ElemCount;
        }
    }
}

void Editor::CursorPositionCallback(const System::Window::Events::CursorPosition& event)
{
    // Set context as current.
    ImGui::SetCurrentContext(m_interface);
    ImGuiIO& io = ImGui::GetIO();

    // Set cursor position.
    io.MousePos.x = (float)event.x;
    io.MousePos.y = (float)event.y;
}

void Editor::MouseButtonCallback(const System::Window::Events::MouseButton& event)
{
    // Set context as current.
    ImGui::SetCurrentContext(m_interface);
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
    ImGui::SetCurrentContext(m_interface);
    ImGuiIO& io = ImGui::GetIO();

    // Set mouse wheel offset.
    io.MouseWheel = (float)event.offset;
}

void Editor::KeyboardKeyCallback(const System::Window::Events::KeyboardKey& event)
{
    // Set context as current.
    ImGui::SetCurrentContext(m_interface);
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
    ImGui::SetCurrentContext(m_interface);
    ImGuiIO& io = ImGui::GetIO();

    // Add text input character.
    io.AddInputCharacter(event.character);
}
