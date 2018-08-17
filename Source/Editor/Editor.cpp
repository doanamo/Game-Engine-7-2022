/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Editor/Editor.hpp"
#include "System/ResourceManager.hpp"
#include "Engine.hpp"
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
    m_engine(nullptr),
    m_interface(nullptr),
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

Editor::Editor(Editor&& other) :
    Editor()
{
    // Call the move assignment.
    *this = std::move(other);
}

Editor& Editor::operator=(Editor&& other)
{
    // Swap class members.
    std::swap(m_engine, other.m_engine);
    std::swap(m_interface, other.m_interface);

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

void Editor::DestroyContext()
{
    // Destroy the user interface context.
    if(m_interface)
    {
        ImGui::DestroyContext(m_interface);
        m_interface = nullptr;
    }
}

bool Editor::Initialize(Engine::Root* engine)
{
    LOG() << "Initializing editor..." << LOG_INDENT();

    // Check if the instance is already initialized.
    VERIFY(!m_initialized, "Editor instance is already initialized!");

    // Reset class instance if initialization fails.
    SCOPE_GUARD_IF(!m_initialized, *this = Editor());

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

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

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
    Graphics::TextureCreateInfo textureInfo;
    textureInfo.width = fontWidth;
    textureInfo.height = fontHeight;
    textureInfo.format = GL_RGBA;
    textureInfo.mipmaps = false;
    textureInfo.data = fontData;

    if(!m_fontTexture.Initialize(&m_engine->renderContext, textureInfo))
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
    Graphics::ShaderLoadInfo shaderInfo;
    shaderInfo.filePath = Build::GetEngineDir() + "Data/Engine/Shaders/Interface.shader";

    m_shader = m_engine->resourceManager.Acquire<Graphics::Shader>(
        shaderInfo.filePath, &m_engine->renderContext, shaderInfo);

    if(m_shader == nullptr)
    {
        LOG_ERROR() << "Could not initialize shader!";
        return false;
    }

    // Success!
    return m_initialized = true;
}

void Engine::Editor::Update(float timeDelta)
{
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

    // Define interface.
    if(ImGui::BeginMainMenuBar())
    {
        if(ImGui::BeginMenu("Engine"))
        {
            ImGui::Separator();

            if(ImGui::MenuItem("Exit"))
            {
                m_engine->window.Close();
            }

            ImGui::EndMenu();
        }

        ImGui::Separator();

        if(ImGui::BeginMenu("View"))
        {
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Debug"))
        {
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void Editor::Draw()
{
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
    m_engine->renderContext.PushState();
    SCOPE_GUARD(m_engine->renderContext.PopState());

    // Prepare rendering state.
    Graphics::RenderState& renderState = m_engine->renderContext.GetState();

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
