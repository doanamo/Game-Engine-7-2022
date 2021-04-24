/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include "Game/GameSystem.hpp"

/*
    Sprite System
*/

namespace Game
{
    class ComponentSystem;

    class SpriteSystem final : public GameSystem
    {
        REFLECTION_ENABLE(SpriteSystem, GameSystem)

    public:
        SpriteSystem();
        ~SpriteSystem() override;

    private:
        bool OnAttach(GameInstance* gameInstance) override;
        void OnTick(float timeDelta) override;

        ComponentSystem* m_componentSystem = nullptr;
    };
}

REFLECTION_TYPE(Game::SpriteSystem, Game::GameSystem)
