/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Common/Event/Receiver.hpp>
#include "Editor/EditorModule.hpp"

namespace Game
{
    class TickTimer;
    class GameFramework;
    class GameInstance;
    class GameState;
};

/*
    Game Instance Editor

    Editor interface for manipulating game instance while it is running.
*/

namespace Editor
{
    class GameInstanceEditor final : public EditorModule
    {
        REFLECTION_ENABLE(GameInstanceEditor, EditorModule)

    public:
        ~GameInstanceEditor();
        GameInstanceEditor();

    private:
        bool OnAttach(const Core::SystemStorage<EditorModule>& editorModules) override;
        bool SubscribeEvents(Game::GameFramework* gameFramework);
        void OnDisplay(float timeDelta) override;
        void OnDisplayMenuBar() override;

        void OnGameStateChanged(const std::shared_ptr<Game::GameState>& gameState);
        void OnTickRequested();
        void OnTickProcessed(float tickTime);

    private:
        Game::TickTimer* m_tickTimer = nullptr;
        Game::GameInstance* m_gameInstance = nullptr;

        bool m_isOpen = false;

        std::vector<float> m_tickTimeHistogram;
        bool m_tickTimeHistogramPaused = false;

        float m_tickRateSlider = 0.0f;
        float m_updateDelaySlider = 0.0f;
        float m_updateDelayValue = 0.0f;
        float m_updateNoiseSlider = 0.0f;
        float m_updateNoiseValue = 0.0f;
        float m_updateFreezeSlider = 1.0f;

        struct Receivers
        {
            Event::Receiver<void(const std::shared_ptr<Game::GameState>&)> gameStateChanged;
            Event::Receiver<void()> tickRequested;
            Event::Receiver<void(float)> tickProcessed;
        } m_receivers;
    };
}

REFLECTION_TYPE(Editor::GameInstanceEditor, Editor::EditorModule)
