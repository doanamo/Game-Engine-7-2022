/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include "Game/GameSystem.hpp"

/*
    Interpolation System
*/

namespace Game
{
    class ComponentSystem;

    class InterpolationSystem final : public GameSystem
    {
        REFLECTION_ENABLE(InterpolationSystem, GameSystem)

    public:
        InterpolationSystem();
        ~InterpolationSystem() override;

    private:
        bool OnAttach(GameInstance* gameInstance) override;
        void OnTick(float timeDelta) override;

        ComponentSystem* m_componentSystem = nullptr;
    };
}

REFLECTION_TYPE(Game::InterpolationSystem, Game::GameSystem)
