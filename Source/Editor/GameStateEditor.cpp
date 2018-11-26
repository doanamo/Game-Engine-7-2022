/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Editor/GameStateEditor.hpp"
#include "Game/GameState.hpp"
using namespace Editor;

GameStateEditor::GameStateEditor() :
    m_gameState(nullptr),
    m_updateRateSlider(0.0f),
    m_initialized(false)
{
    m_receivers.gameStateDestructed.Bind<GameStateEditor, &GameStateEditor::OnGameStateDestructed>(this);
    m_receivers.gameStateUpdateCalled.Bind<GameStateEditor, &GameStateEditor::OnGameStateUpdateCalled>(this);
    m_receivers.gameStateUpdated.Bind<GameStateEditor, &GameStateEditor::OnGameStateUpdated>(this);
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
    std::swap(m_receivers, other.m_receivers);
    std::swap(m_gameState, other.m_gameState);
    std::swap(m_updateRateSlider, other.m_updateRateSlider);
    std::swap(m_updateTimeHistogram, other.m_updateTimeHistogram);
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool GameStateEditor::Initialize()
{
    LOG() << "Initializing game state editor..." << LOG_INDENT();

    // Make sure class instance has not been initialized yet.
    ASSERT(!m_initialized, "Game state editor instance has already been initialized!");

    // Set histogram size.
    m_updateTimeHistogram.resize(100, 0.0f);

    // Success!
    return m_initialized = true;
}

void GameStateEditor::Update(float timeDelta)
{
    ASSERT(m_initialized, "Game state editor has not been initialized yet!");

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
                // Show update controls.
                float currentUpdateTime = m_gameState->GetUpdateTime();
                float currentUpdateRate = 1.0f / currentUpdateTime;
                ImGui::BulletText("Update time: %fs (%.1f update rate)", currentUpdateTime, currentUpdateRate);

                ImGui::SliderFloat("##UpdateRateSlider", &m_updateRateSlider, 1.0f, 100.0f, "%.1f updates per second", 2.0f);
                ImGui::SameLine();

                if(ImGui::Button("Apply"))
                {
                    float newUpdateTime = 1.0f / m_updateRateSlider;
                    m_gameState->SetUpdateTime(newUpdateTime);
                }

                // Show update histogram.
                ImGui::BulletText("Update time histogram:");
                ImGui::PlotHistogram("##UpdateTimeHistogram", 
                    &m_updateTimeHistogram[0], 
                    m_updateTimeHistogram.size(),
                    0, "", FLT_MAX, FLT_MAX, 
                    ImVec2(0, 100));

                ImGui::TreePop();
            }
        }
    }
    ImGui::End();
}

void GameStateEditor::SetGameState(Game::GameState* gameState)
{
    ASSERT(m_initialized, "Game state editor has not been initialized yet!");

    if(gameState)
    {
        // Replace with new game state reference.
        m_gameState = gameState;

        // Subscribe to game state events.
        m_receivers.gameStateDestructed.Subscribe(gameState->events.instanceDestructed);
        m_receivers.gameStateUpdateCalled.Subscribe(gameState->events.updateCalled);
        m_receivers.gameStateUpdated.Subscribe(gameState->events.stateUpdated);

        // Update update time slider value.
        m_updateRateSlider = 1.0f / m_gameState->GetUpdateTime();

        // Clear update time histogram.
        for(auto& updateTime : m_updateTimeHistogram)
        {
            updateTime = 0.0f;
        }
    }
    else
    {
        // Remove game state reference.
        m_gameState = nullptr;
        m_receivers = Receivers();
    }
}

void Editor::GameStateEditor::OnGameStateDestructed()
{
    ASSERT(m_initialized, "Game state editor has not been initialized yet!");

    // Reset game state reference.
    m_gameState = nullptr;
    m_receivers = Receivers();
}

void Editor::GameStateEditor::OnGameStateUpdateCalled()
{
    ASSERT(m_initialized, "Game state editor has not been initialized yet!");

    // Rotate update time histogram array to the right.
    if(m_updateTimeHistogram.size() >= 2)
    {
        std::rotate(m_updateTimeHistogram.begin(), m_updateTimeHistogram.begin() + 1, m_updateTimeHistogram.end());
    }

    // Reset the first value that will accumulate new update time values.
    if(!m_updateTimeHistogram.empty())
    {
        m_updateTimeHistogram.back() = 0.0f;
    }
}

void Editor::GameStateEditor::OnGameStateUpdated(float updateTime)
{
    ASSERT(m_initialized, "Game state editor has not been initialized yet!");

    // Accumulate new update time.
    if(!m_updateTimeHistogram.empty())
    {
        m_updateTimeHistogram.back() += updateTime;
    }
}

Game::GameState* GameStateEditor::GetGameState() const
{
    return m_gameState;
}
