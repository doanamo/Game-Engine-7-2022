/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Editor/Precompiled.hpp"
#include "Editor/Modules/GameInstanceEditor.hpp"
#include <Core/SystemStorage.hpp>
#include <Game/GameFramework.hpp>
#include <Game/GameInstance.hpp>
#include <Game/GameState.hpp>
#include <Game/TickTimer.hpp>
using namespace Editor;

namespace
{
    const char* LogAttachFailed = "Failed to attach game instance editor module! {}";
}

GameInstanceEditor::GameInstanceEditor()
{
    m_receivers.gameStateChanged.Bind<GameInstanceEditor,
        &GameInstanceEditor::OnGameStateChanged>(this);
    m_receivers.tickRequested.Bind<GameInstanceEditor,
        &GameInstanceEditor::OnTickRequested>(this);
    m_receivers.tickProcessed.Bind<GameInstanceEditor,
        &GameInstanceEditor::OnTickProcessed>(this);
}

GameInstanceEditor::~GameInstanceEditor() = default;

bool GameInstanceEditor::OnAttach(const Core::SystemStorage<EditorModule>& editorModules)
{
    // Retrieve needed systems.
    auto* editorContext = editorModules.Locate<EditorModuleContext>();
    if(editorContext == nullptr)
    {
        LOG_ERROR(LogAttachFailed, "Could not locate editor module context.");
        return false;
    }

    auto& engineSystems = editorContext->GetEngineSystems();
    auto* gameFramework = engineSystems.Locate<Game::GameFramework>();
    if(gameFramework == nullptr)
    {
        LOG_ERROR(LogAttachFailed, "Could not locate game framework system.");
        return false;
    }

    // Subscribe events.
    if(!SubscribeEvents(gameFramework))
    {
        LOG_ERROR(LogAttachFailed, "Could not subscribe to game framework events.");
        return false;
    }

    // Allocate histogram buffer.
    m_tickTimeHistogram.resize(100, 0.0f);

    return true;
}

bool GameInstanceEditor::SubscribeEvents(Game::GameFramework* gameFramework)
{
    bool subscriptionResults = true;

    Game::GameFramework::Events& events = gameFramework->events;
    subscriptionResults &= m_receivers.gameStateChanged.Subscribe(events.gameStateChanged);
    subscriptionResults &= m_receivers.tickRequested.Subscribe(events.tickRequested);
    subscriptionResults &= m_receivers.tickProcessed.Subscribe(events.tickProcessed);

    return subscriptionResults;
}

void GameInstanceEditor::OnDisplay(float timeDelta)
{
    if(!m_isOpen)
        return;

    if(!m_gameInstance)
        return;

    ImGui::SetNextWindowSizeConstraints(ImVec2(200.0f, 0.0f), ImVec2(FLT_MAX, FLT_MAX));
    if(ImGui::Begin("Game Framework", &m_isOpen, ImGuiWindowFlags_AlwaysAutoResize))
    {
        if(ImGui::CollapsingHeader("Core"))
        {
            if(m_tickTimer && ImGui::TreeNode("Tick Timer"))
            {
                // Tick controls.
                float currentTickTime = m_tickTimer->GetTickSeconds();
                float currentTickRate = 1.0f / currentTickTime;
                ImGui::BulletText("Tick time: %fs (%.1f tick rate)",
                    currentTickTime, currentTickRate);

                ImGui::SliderFloat("##TickRateSlider", &m_tickRateSlider,
                    1.0f, 100.0f, "%.1f ticks(s) per second", ImGuiSliderFlags_Logarithmic);

                ImGui::SameLine();
                if(ImGui::Button("Apply##TickTimeApply"))
                {
                    m_tickTimer->SetTickSeconds(1.0f / m_tickRateSlider);
                }

                // Tick histogram.
                ImGui::BulletText("Tick time histogram:");
                ImGui::PlotHistogram("##TickTimeHistogram",
                    &m_tickTimeHistogram[0], (int)m_tickTimeHistogram.size(),
                    0, "", FLT_MAX, FLT_MAX, ImVec2(0, 100));

                // Histogram statistics.
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

                // Time delay slider.
                ImGui::BulletText("Update time delay: %0.3fs", m_updateDelayValue);
                ImGui::SliderFloat("##UpdateDelaySlider", &m_updateDelaySlider,
                    0.0f, 1.0f, "%0.3fs delay", ImGuiSliderFlags_Logarithmic);

                ImGui::SameLine();
                if(ImGui::Button("Apply##UpdateDelayApply"))
                {
                    m_updateDelayValue = m_updateDelaySlider;
                }

                if(m_updateDelayValue > 0.0f)
                {
                    int sleepTime = static_cast<int>(m_updateDelayValue * 1000.0f);
                    std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
                }

                // Time noise slider.
                ImGui::BulletText("Update time noise: %0.3fs", m_updateNoiseValue);
                ImGui::SliderFloat("##UpdateNoiseSlider", &m_updateNoiseSlider,
                    0.0f, 1.0f, "%0.3fs noise", ImGuiSliderFlags_Logarithmic);

                ImGui::SameLine();
                if(ImGui::Button("Apply##UpdateNoiseApply"))
                {
                    m_updateNoiseValue = m_updateNoiseSlider;
                }

                if(m_updateNoiseValue > 0.0f)
                {
                    int sleepTime = std::rand() % static_cast<int>(m_updateNoiseValue * 1000.0f);
                    std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
                }

                // Time freeze slider.
                ImGui::BulletText("Update time freeze:");
                ImGui::SliderFloat("##UpdateFreezeSlider", &m_updateFreezeSlider,
                    0.1f, 10.0f, "%.1fs freeze", ImGuiSliderFlags_Logarithmic);

                ImGui::SameLine();
                if(ImGui::Button("Freeze"))
                {
                    int sleepTime = static_cast<int>(m_updateFreezeSlider * 1000.0f);
                    std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
                }

                ImGui::TreePop();
            }

            if(m_gameInstance && ImGui::TreeNode("Game Instance"))
            {
                ImGui::TreePop();
            }
        }
    }

    ImGui::End();
}

void GameInstanceEditor::OnDisplayMenuBar()
{
    if(ImGui::BeginMenu("Game"))
    {
        ImGui::MenuItem("Game Framework", "", &m_isOpen, true);
        ImGui::EndMenu();
    }
}

void GameInstanceEditor::OnGameStateChanged(const std::shared_ptr<Game::GameState>& gameState)
{
    // Update references.
    m_tickTimer = gameState ? gameState->GetTickTimer() : nullptr;
    m_gameInstance = gameState ? gameState->GetGameInstance() : nullptr;

    // Reset controls.
    if(m_tickTimer)
    {
        m_tickRateSlider = 1.0f / m_tickTimer->GetTickSeconds();

        for(auto& tickTime : m_tickTimeHistogram)
        {
            tickTime = 0.0f;
        }
    }
}

void GameInstanceEditor::OnTickRequested()
{
    if(m_tickTimeHistogramPaused)
        return;

    // Rotate out oldest histogram value.
    if(m_tickTimeHistogram.size() >= 2)
    {
        std::rotate(m_tickTimeHistogram.begin(),
            m_tickTimeHistogram.begin() + 1, m_tickTimeHistogram.end());
    }

    if(!m_tickTimeHistogram.empty())
    {
        m_tickTimeHistogram.back() = 0.0f;
    }
}

void GameInstanceEditor::OnTickProcessed(float tickTime)
{
    if(m_tickTimeHistogramPaused)
        return;

    // Add time to newest histogram value.
    if(!m_tickTimeHistogram.empty())
    {
        m_tickTimeHistogram.back() += tickTime;
    }
}
