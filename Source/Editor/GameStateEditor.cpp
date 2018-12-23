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
    m_updateTimeHistogramPaused(false),
    m_updateDelaySlider(0.0f),
    m_updateDelayValue(0.0f),
    m_updateNoiseSlider(0.0f),
    m_updateNoiseValue(0.0f),
    m_updateFreezeSlider(1.0f),
    m_initialized(false)
{
    m_receivers.gameStateDestructed.Bind<GameStateEditor, &GameStateEditor::OnGameStateDestructed>(this);
    m_receivers.gameStateUpdateCalled.Bind<GameStateEditor, &GameStateEditor::OnGameStateUpdateCalled>(this);
    m_receivers.gameStateUpdated.Bind<GameStateEditor, &GameStateEditor::OnGameStateUpdated>(this);
}

GameStateEditor::~GameStateEditor()
{
}

GameStateEditor::GameStateEditor(GameStateEditor&& other) :
    GameStateEditor()
{
    *this = std::move(other);
}

GameStateEditor& GameStateEditor::operator=(GameStateEditor&& other)
{
    std::swap(m_receivers, other.m_receivers);
    std::swap(m_gameState, other.m_gameState);
    std::swap(m_updateRateSlider, other.m_updateRateSlider);
    std::swap(m_updateTimeHistogram, other.m_updateTimeHistogram);
    std::swap(m_updateTimeHistogramPaused, other.m_updateTimeHistogramPaused);
    std::swap(m_updateDelaySlider, other.m_updateDelaySlider);
    std::swap(m_updateDelayValue, other.m_updateDelayValue);
    std::swap(m_updateNoiseSlider, other.m_updateNoiseSlider);
    std::swap(m_updateNoiseValue, other.m_updateNoiseValue);
    std::swap(m_updateFreezeSlider, other.m_updateFreezeSlider);
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
                ImGui::BulletText("Update time: %fs (%.1f update rate)",
                    currentUpdateTime, currentUpdateRate);

                ImGui::SliderFloat("##UpdateRateSlider", &m_updateRateSlider,
                    1.0f, 100.0f, "%.1f update(s) per second", 2.0f);

                ImGui::SameLine();
                if(ImGui::Button("Apply##UpdateTimeApply"))
                {
                    Game::GameState::Events::ChangeUpdateTime changeUpdateTime;
                    changeUpdateTime.updateTime = 1.0f / m_updateRateSlider;
                    m_gameState->PushEvent(changeUpdateTime);
                }

                // Show update histogram.
                ImGui::BulletText("Update time histogram:");
                ImGui::PlotHistogram("##UpdateTimeHistogram", 
                    &m_updateTimeHistogram[0], m_updateTimeHistogram.size(),
                    0, "", FLT_MAX, FLT_MAX, ImVec2(0, 100));

                // Process histogram statistics.
                int updateTimeValues = 0;
                float updateTimeMinimum = FLT_MAX;
                float updateTimeMaximum = 0.0f;
                float updateTimeAverage = 0.0f;

                for(float& updateTime : m_updateTimeHistogram)
                {
                    if(updateTime == 0.0f)
                        continue;

                    updateTimeValues += 1;
                    updateTimeMinimum = std::min(updateTime, updateTimeMinimum);
                    updateTimeMaximum = std::max(updateTime, updateTimeMaximum);
                    updateTimeAverage += updateTime;
                }

                if(updateTimeMinimum == FLT_MAX)
                    updateTimeMinimum = 0.0f;

                updateTimeAverage /= updateTimeValues;

                // Print histogram statistics.
                ImGui::SameLine();
                ImGui::BeginGroup();
                {
                    ImGui::Text("Min: %0.3f", updateTimeMinimum);
                    ImGui::Text("Max: %0.3f", updateTimeMaximum);
                    ImGui::Text("Avg: %0.3f", updateTimeAverage);

                    ImGui::PushID("UpdateTimeHistogramToggle");
                    if(ImGui::Button(m_updateTimeHistogramPaused ? "Resume" : "Pause"))
                    {
                        m_updateTimeHistogramPaused = !m_updateTimeHistogramPaused;
                    }
                    ImGui::PopID();
                }
                ImGui::EndGroup();

                // Update time delay slider.
                ImGui::BulletText("Update time delay: %0.3fs", m_updateDelayValue);
                ImGui::SliderFloat("##UpdateDelaySlider", &m_updateDelaySlider,
                    0.0f, 1.0f, "%0.3fs delay", 6.0f);

                ImGui::SameLine();
                if(ImGui::Button("Apply##UpdateDelayApply"))
                {
                    m_updateDelayValue = m_updateDelaySlider;
                }

                if(m_updateDelayValue > 0.0f)
                {
                    int sleepTime = (int)(m_updateDelayValue * 1000.0f);
                    std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
                }

                // Update time noise slider.
                ImGui::BulletText("Update time noise: %0.3fs", m_updateNoiseValue);
                ImGui::SliderFloat("##UpdateNoiseSlider", &m_updateNoiseSlider,
                    0.0f, 1.0f, "%0.3fs noise", 4.0f);

                ImGui::SameLine();
                if(ImGui::Button("Apply##UpdateNoiseApply"))
                {
                    m_updateNoiseValue = m_updateNoiseSlider;
                }

                if(m_updateNoiseValue > 0.0f)
                {
                    int sleepTime = std::rand() % (int)(m_updateNoiseValue * 1000.0f);
                    std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
                }

                // Update time freeze slider.
                ImGui::BulletText("Update time freeze:");
                ImGui::SliderFloat("##UpdateFreezeSlider", &m_updateFreezeSlider,
                    0.1f, 10.0f, "%.1fs freeze", 2.0f);

                ImGui::SameLine();
                if(ImGui::Button("Freeze"))
                {
                    int sleepTime = (int)(m_updateFreezeSlider * 1000.0f);
                    std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
                }

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

        // Subscribe to game state dispatchers.
        m_receivers.gameStateDestructed.Subscribe(gameState->dispatchers.instanceDestructed);
        m_receivers.gameStateUpdateCalled.Subscribe(gameState->dispatchers.updateCalled);
        m_receivers.gameStateUpdated.Subscribe(gameState->dispatchers.stateUpdated);

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

    // Do not process histogram data if paused.
    if(m_updateTimeHistogramPaused)
        return;

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

    // Do not process histogram data if paused.
    if(m_updateTimeHistogramPaused)
        return;

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
