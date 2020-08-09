/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <Event/Receiver.hpp>
#include <Core/ServiceStorage.hpp>

namespace Game
{
    class GameFramework;
    class GameInstance;
    class GameState;
};

/*
    Game Instance Editor
*/

namespace Editor
{
    class GameInstanceEditor final : private Common::NonCopyable
    {
    public:
        struct CreateFromParams
        {
            const Core::ServiceStorage* services = nullptr;
        };

        enum class CreateErrors
        {
            InvalidArgument,
        };

        using CreateResult = Common::Result<std::unique_ptr<GameInstanceEditor>, CreateErrors>;
        static CreateResult Create(const CreateFromParams& params);

    public:
        ~GameInstanceEditor();

        void Update(float timeDelta);

        bool mainWindowOpen = false;

    private:
        GameInstanceEditor();

        struct Receivers
        {
            Event::Receiver<void(const std::shared_ptr<Game::GameState>&)> gameStateChanged;
            Event::Receiver<void()> gameInstanceDestroyed;
            Event::Receiver<void()> gameInstanceTickRequested;
            Event::Receiver<void(float)> gameInstanceTickProcessed;
        } m_receivers;

        void OnGameStateChanged(const std::shared_ptr<Game::GameState>& gameState);
        void OnGameInstanceDestroyed();
        void OnGameInstanceTickRequested();
        void OnGameInstanceTickProcessed(float tickTime);

        Game::GameInstance* m_gameInstance = nullptr;

        std::vector<float> m_tickTimeHistogram;
        bool m_tickTimeHistogramPaused = false;

        float m_tickRateSlider = 0.0f;
        float m_updateDelaySlider = 0.0f;
        float m_updateDelayValue = 0.0f;
        float m_updateNoiseSlider = 0.0f;
        float m_updateNoiseValue = 0.0f;
        float m_updateFreezeSlider = 1.0f;
    };
}
