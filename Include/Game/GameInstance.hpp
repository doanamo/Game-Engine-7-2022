/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <any>
#include <Common/Event/Queue.hpp>
#include "Game/TickTimer.hpp"
#include "Game/EntitySystem.hpp"
#include "Game/ComponentSystem.hpp"
#include "Game/Systems/IdentitySystem.hpp"
#include "Game/Systems/InterpolationSystem.hpp"
#include "Game/Systems/SpriteSystem.hpp"

/*
    Game Instance
*/

namespace Game
{
    class GameInstance final : private Common::NonCopyable
    {
    public:
        enum class CreateErrors
        {
            FailedSubsystemCreation,
        };

        using CreateResult = Common::Result<std::unique_ptr<GameInstance>, CreateErrors>;
        static CreateResult Create();

    public:
        ~GameInstance();

        void PreTick(const float tickTime);
        void PostTick(const float tickTime);

    public:
        std::unique_ptr<EntitySystem> entitySystem;
        std::unique_ptr<ComponentSystem> componentSystem;
        std::unique_ptr<IdentitySystem> identitySystem;
        std::unique_ptr<InterpolationSystem> interpolationSystem;
        std::unique_ptr<SpriteSystem> spriteSystem;

    private:
        GameInstance();
    };
}
