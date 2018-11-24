/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Editor/GameStateEditor.hpp"
#include "Game/GameState.hpp"
using namespace Editor;

GameStateEditor::GameStateEditor() :
    m_gameState(nullptr),
    m_initialized(false)
{
}

GameStateEditor::~GameStateEditor()
{
}

GameStateEditor::GameStateEditor(GameStateEditor&& other)
{
    *this = std::move(other);
}

GameStateEditor& GameStateEditor::operator=(GameStateEditor&& other)
{
    std::swap(m_gameState, other.m_gameState);
    std::swap(m_receiverDestruction, other.m_receiverDestruction);
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool GameStateEditor::Initialize()
{
    LOG() << "Initializing game state editor..." << LOG_INDENT();

    // Make sure class instance has not been initialized yet.
    ASSERT(!m_initialized, "Game state editor instance has already been initialized!");

    // Setup receiver to clear game state when it gets destructed.
    m_receiverDestruction.Bind([this]()
    {
        m_gameState = nullptr;
    });

    // Success!
    return m_initialized = true;
}

void GameStateEditor::Update(float timeDelta)
{
    // Do not create a window if game state reference is not set.
    if(!m_gameState)
        return;

    // Show game state window.
    ImGui::SetNextWindowSizeConstraints(ImVec2(200.0f, 0.0f), ImVec2(FLT_MAX, FLT_MAX));

    if(ImGui::Begin("Game State", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        if(ImGui::CollapsingHeader("Core"))
        {
            if(ImGui::TreeNode("Update Timer"))
            {
                // Show timer controls.
                static float updatesPerSecond = 10.0f;
                static float secondsPerUpdate = 1.0f / updatesPerSecond;
                ImGui::BulletText("Update time: %fs (%.1f update rate)", secondsPerUpdate, updatesPerSecond);
                ImGui::SliderFloat("##UpdateRateSlider", &updatesPerSecond, 1.0f, 100.0f, "%.1f updates per second", 2.0f);
                ImGui::SameLine();
                ImGui::Button("Apply");

                // Show timer histogram.
                ImGui::BulletText("Update histogram:");
                ImGui::PlotHistogram("##UpdateHistogram", nullptr, nullptr, 0);

                ImGui::TreePop();
            }
        }
    }
    ImGui::End();
}

void GameStateEditor::SetGameState(Game::GameState* gameState)
{
    // Replace with new game state reference.
    if(gameState)
    {
        m_gameState = gameState;
        m_receiverDestruction.Subscribe(gameState->events.instanceDestruction);
    }
    else
    {
        m_gameState = nullptr;
        m_receiverDestruction.Unsubscribe();
    }
}

Game::GameState* GameStateEditor::GetGameState() const
{
    return m_gameState;
}
