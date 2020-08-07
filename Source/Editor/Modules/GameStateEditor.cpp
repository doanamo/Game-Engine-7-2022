/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "../Precompiled.hpp"
#include "Editor/Modules/GameStateEditor.hpp"
#include <Game/GameFramework.hpp>
#include <Game/GameState.hpp>
using namespace Editor;

GameStateEditor::GameStateEditor()
{
    m_receivers.gameStateChanged.Bind<GameStateEditor, &GameStateEditor::OnGameStateChanged>(this);
    m_receivers.gameStateDestructed.Bind<GameStateEditor, &GameStateEditor::OnGameStateDestructed>(this);
    m_receivers.gameStateTickCalled.Bind<GameStateEditor, &GameStateEditor::OnGameStateTickCalled>(this);
    m_receivers.gameStateTickProcessed.Bind<GameStateEditor, &GameStateEditor::OnGameStateTickProcessed>(this);
}

GameStateEditor::~GameStateEditor() = default;

GameStateEditor::CreateResult GameStateEditor::Create(const CreateFromParams& params)
{
    LOG("Creating game state editor...");
    LOG_SCOPED_INDENT();

    // Check arguments.
    CHECK_ARGUMENT_OR_RETURN(params.services != nullptr, Common::Failure(CreateErrors::InvalidArgument));

    // Acquire game framework service.
    Game::GameFramework* gameFramework = params.services->GetGameFramework();

    // Create instance.
    auto instance = std::unique_ptr<GameStateEditor>(new GameStateEditor());

    // Subscribe to game state being changed.
    instance->m_receivers.gameStateChanged.Subscribe(gameFramework->events.gameStateChanged);

    // Set histogram size.
    instance->m_tickTimeHistogram.resize(100, 0.0f);

    // Success!
    return Common::Success(std::move(instance));
}

void GameStateEditor::Update(float timeDelta)
{
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
                if(ImGui::TreeNode("Tick Control"))
                {
                    // Show tick controls.
                    float currentTickTime = m_gameState->tickTimer->GetTickSeconds();
                    float currentTickRate = 1.0f / currentTickTime;
                    ImGui::BulletText("Tick time: %fs (%.1f tick rate)",
                        currentTickTime, currentTickRate);

                    ImGui::SliderFloat("##TickRateSlider", &m_tickRateSlider,
                        1.0f, 100.0f, "%.1f ticks(s) per second", 2.0f);

                    ImGui::SameLine();
                    if(ImGui::Button("Apply##TickTimeApply"))
                    {
                        m_gameState->tickTimer->SetTickSeconds(1.0f / m_tickRateSlider);
                    }

                    // Show tick histogram.
                    ImGui::BulletText("Tick time histogram:");
                    ImGui::PlotHistogram("##TickTimeHistogram",
                        &m_tickTimeHistogram[0], (int)m_tickTimeHistogram.size(),
                        0, "", FLT_MAX, FLT_MAX, ImVec2(0, 100));

                    // Process histogram statistics.
                    int tickTimeValues = 0;
                    float tickTimeMinimum = FLT_MAX;
                    float tickTimeMaximum = 0.0f;
                    float tickTimeAverage = 0.0f;

                    for(float tickTime : m_tickTimeHistogram)
                    {
                        if(tickTime == 0.0f)
                            continue;

                        tickTimeValues += 1;
                        tickTimeMinimum = std::min(tickTime, tickTimeMinimum);
                        tickTimeMaximum = std::max(tickTime, tickTimeMaximum);
                        tickTimeAverage += tickTime;
                    }

                    if(tickTimeMinimum == FLT_MAX)
                        tickTimeMinimum = 0.0f;

                    if(tickTimeValues != 0)
                    {
                        tickTimeAverage /= tickTimeValues;
                    }

                    // Print histogram statistics.
                    ImGui::SameLine();
                    ImGui::BeginGroup();
                    {
                        ImGui::Text("Min: %0.3f", tickTimeMinimum);
                        ImGui::Text("Max: %0.3f", tickTimeMaximum);
                        ImGui::Text("Avg: %0.3f", tickTimeAverage);

                        ImGui::PushID("TickTimeHistogramToggle");
                        if(ImGui::Button(m_tickTimeHistogramPaused ? "Resume" : "Pause"))
                        {
                            m_tickTimeHistogramPaused = !m_tickTimeHistogramPaused;
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
    if(gameState)
    {
        // Replace with new game state reference.
        m_gameState = gameState.get();

        // Subscribe to game state dispatchers.
        m_receivers.gameStateDestructed.Subscribe(gameState->events.instanceDestructed);
        m_receivers.gameStateTickCalled.Subscribe(gameState->events.tickCalled);
        m_receivers.gameStateTickProcessed.Subscribe(gameState->events.tickProcessed);

        // Update tick time slider value.
        m_tickRateSlider = 1.0f / m_gameState->tickTimer->GetTickSeconds();

        // Clear tick time histogram.
        for(auto& tickTime : m_tickTimeHistogram)
        {
            tickTime = 0.0f;
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
    // Reset game state reference.
    m_gameState = nullptr;
    m_receivers = Receivers();
}

void GameStateEditor::OnGameStateTickCalled()
{
    // Do not process histogram data if paused.
    if(m_tickTimeHistogramPaused)
        return;

    // Rotate tick time histogram array to the right.
    if(m_tickTimeHistogram.size() >= 2)
    {
        std::rotate(m_tickTimeHistogram.begin(), m_tickTimeHistogram.begin() + 1, m_tickTimeHistogram.end());
    }

    // Reset first value that will accumulate new tick time values.
    if(!m_tickTimeHistogram.empty())
    {
        m_tickTimeHistogram.back() = 0.0f;
    }
}

void GameStateEditor::OnGameStateTickProcessed(float tickTime)
{
    // Do not process histogram data if paused.
    if(m_tickTimeHistogramPaused)
        return;

    // Accumulate new tick time.
    if(!m_tickTimeHistogram.empty())
    {
        m_tickTimeHistogram.back() += tickTime;
    }
}

Game::GameState* GameStateEditor::GetGameState() const
{
    return m_gameState;
}
