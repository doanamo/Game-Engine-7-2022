/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "GameStates/SpriteDemo.hpp"
#include <Game/GameFramework.hpp>

int main()
{
    // Main entry point where engine instance is created
    // and where game state is added to be run by engine.

    Engine::Root::CreateFromParams engineParams;
    engineParams.maxUpdateDelta = 1.0f;

    if(auto engine = Engine::Root::Create(engineParams).UnwrapOr(nullptr))
    {
        std::shared_ptr<Game::GameState> defaultGameState = SpriteDemo::Create(engine.get()).UnwrapOr(nullptr);
        engine->GetServices().GetGameFramework()->ChangeGameState(defaultGameState);
        return engine->Run();
    }
    else
    {
        LOG_ERROR("Could not create engine!");
        return -1;
    }
};
