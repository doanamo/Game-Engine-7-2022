/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Precompiled.hpp"
#include "GameStates/SpriteDemo.hpp"
#include <Core/ConfigTypes.hpp>
#include <Game/GameFramework.hpp>

int main()
{
    /*
        Main entry point where engine instance is created
        and where game state is added to be run by engine.
    */

    const Core::ConfigVariableArray configVars =
    {
        { "timer.maxUpdateDelta", "1.0f" },
    };

    if(auto engine = Engine::Root::Create(configVars).UnwrapOr(nullptr))
    {
        std::shared_ptr<Game::GameState> defaultGameState =
            SpriteDemo::Create(engine.get()).UnwrapOr(nullptr);
        engine->GetSystems().Locate<Game::GameFramework>()->ChangeGameState(defaultGameState);
        return engine->Run();
    }
    else
    {
        LOG_ERROR("Could not create engine!");
        return -1;
    }
};
