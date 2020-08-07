/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <any>
#include <Event/Queue.hpp>
#include <Event/Broker.hpp>
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

        bool Tick(const System::Timer& timer);

    public:
        std::unique_ptr<TickTimer> tickTimer;
        std::unique_ptr<EntitySystem> entitySystem;
        std::unique_ptr<ComponentSystem> componentSystem;

        std::unique_ptr<IdentitySystem> identitySystem;
        std::unique_ptr<InterpolationSystem> interpolationSystem;
        std::unique_ptr<SpriteSystem> spriteSystem;

        struct Events
        {
            // Called when instance is destroyed.
            Event::Dispatcher<void()> instanceDestroyed;

            // Called when tick method is called.
            // This does not mean that the state was actually ticked.
            Event::Dispatcher<void()> tickRequested;

            // Called when state had its tick processed.
            // Event can be dispatched multiple times during the same tick method call.
            // This is also good time to run custom tick logic in response.
            Event::Dispatcher<void(float)> tickProcessed;
        } events;

    private:
        GameInstance();
    };
}
