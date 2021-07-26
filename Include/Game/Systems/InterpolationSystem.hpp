/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include "Game/GameSystem.hpp"

/*
    Interpolation System

    Responsible for interpolation of position/rotation/scale in entities between game ticks.
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
        bool OnAttach(const GameSystemStorage& gameSystems) override;
        void OnTick(float timeDelta) override;

    private:
        ComponentSystem* m_componentSystem = nullptr;
    };
}

REFLECTION_TYPE(Game::InterpolationSystem, Game::GameSystem)
