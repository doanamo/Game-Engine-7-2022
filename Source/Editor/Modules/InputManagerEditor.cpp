/*
    Copyright (c) 2018-2019 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Editor/Modules/InputManagerEditor.hpp"
using namespace Editor;

InputManagerEditor::InputManagerEditor() :
    mainWindowOpen(false),
    m_engine(nullptr),
    m_initialized(false)
{
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
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool InputManagerEditor::Initialize(Engine::Root* engine)
{
    LOG() << "Initializing input manager editor..." << LOG_INDENT();

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
                    ImGui::TreePop();
                }
            }
        }

        ImGui::End();
    }
}
