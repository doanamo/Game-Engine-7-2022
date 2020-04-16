/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Editor/Modules/GameStateEditor.hpp"
#include <Game/GameFramework.hpp>
#include <Game/GameState.hpp>
using namespace Editor;

GameStateEditor::GameStateEditor()
{
    m_receivers.gameStateChanged.Bind<GameStateEditor, &GameStateEditor::OnGameStateChanged>(this);
    m_receivers.gameStateDestructed.Bind<GameStateEditor, &GameStateEditor::OnGameStateDestructed>(this);
    m_receivers.gameStateUpdateCalled.Bind<GameStateEditor, &GameStateEditor::OnGameStateUpdateCalled>(this);
    m_receivers.gameStateUpdateProcessed.Bind<GameStateEditor, &GameStateEditor::OnGameStateUpdateProcessed>(this);
}

GameStateEditor::~GameStateEditor() = default;

GameStateEditor::InitializeResult GameStateEditor::Initialize(const InitializeFromParams& params)
{
    LOG("Initializing game state editor...");
    LOG_SCOPED_INDENT();

    // Setup initialization guard.
    VERIFY(!m_initialized, "Instance has already been initialized!");
    SCOPE_GUARD_IF(!m_initialized, this->Reset());

    // Check arguments.
    CHECK_ARGUMENT_OR_RETURN(params.gameFramework != nullptr, Failure(InitializeErrors::InvalidArgument));

    // Subscribe to game state being changed.
    m_receivers.gameStateChanged.Subscribe(params.gameFramework->events.gameStateChanged);

    // Set histogram size.
    m_updateTimeHistogram.resize(100, 0.0f);

    // Success!
    m_initialized = true;
    return Success();
}

void GameStateEditor::Update(float timeDelta)
{
    ASSERT(m_initialized, "Game state editor has not been initialized!");

    // Do not create a window if game state reference is not set.
    if(!m_gameState)
    {
        mainWindowOpen = false;
        return;
    }

    // Show game state window.
    if(mainWindowOpen)
    {
        ImGui::SetNextWindowSizeConstraints(ImVec2(200.0f, 0.0f), ImVec2(FLT_MAX, FLT_MAX));

        if(ImGui::Begin("Game State", &mainWindowOpen, ImGuiWindowFlags_AlwaysAutoResize))
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
                        &m_updateTimeHistogram[0], (int)m_updateTimeHistogram.size(),
                        0, "", FLT_MAX, FLT_MAX, ImVec2(0, 100));

                    // Process histogram statistics.
                    int updateTimeValues = 0;
                    float updateTimeMinimum = FLT_MAX;
                    float updateTimeMaximum = 0.0f;
                    float updateTimeAverage = 0.0f;

                    for(float updateTime : m_updateTimeHistogram)
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

                    if(updateTimeValues != 0)
                    {
                        updateTimeAverage /= updateTimeValues;
                    }

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
}

void GameStateEditor::OnGameStateChanged(const std::shared_ptr<Game::GameState>& gameState)
{
    ASSERT(m_initialized, "Game state editor has not been initialized!");

    if(gameState)
    {
        // Replace with new game state reference.
        m_gameState = gameState.get();

        // Subscribe to game state dispatchers.
        m_receivers.gameStateDestructed.Subscribe(gameState->events.instanceDestructed);
        m_receivers.gameStateUpdateCalled.Subscribe(gameState->events.updateCalled);
        m_receivers.gameStateUpdateProcessed.Subscribe(gameState->events.updateProcessed);

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

void GameStateEditor::OnGameStateDestructed()
{
    ASSERT(m_initialized, "Game state editor has not been initialized!");

    // Reset game state reference.
    m_gameState = nullptr;
    m_receivers = Receivers();
}

void GameStateEditor::OnGameStateUpdateCalled()
{
    ASSERT(m_initialized, "Game state editor has not been initialized!");

    // Do not process histogram data if paused.
    if(m_updateTimeHistogramPaused)
        return;

    // Rotate update time histogram array to the right.
    if(m_updateTimeHistogram.size() >= 2)
    {
        std::rotate(m_updateTimeHistogram.begin(), m_updateTimeHistogram.begin() + 1, m_updateTimeHistogram.end());
    }

    // Reset first value that will accumulate new update time values.
    if(!m_updateTimeHistogram.empty())
    {
        m_updateTimeHistogram.back() = 0.0f;
    }
}

void GameStateEditor::OnGameStateUpdateProcessed(float updateTime)
{
    ASSERT(m_initialized, "Game state editor has not been initialized!");

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
    ASSERT(m_initialized, "Game state editor has not been initialized!");
    return m_gameState;
}
