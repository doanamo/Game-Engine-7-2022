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
        { "core.foregroundFpsLimit", "0.0" },
        { "core.backgroundFpsLimit", "10.0" },
        { "system.maxUpdateDelta", "1.0" },
        { "render.spriteBatchSize", "128" },
    };

    if(auto engine = Engine::Root::Create(configVars).UnwrapOr(nullptr))
    {
        std::shared_ptr<Game::GameState> defaultGameState =
            SpriteDemo::Create(engine.get()).UnwrapOr(nullptr);

        auto& gameFramework = engine->GetSystems().Locate<Game::GameFramework>();
        gameFramework.ChangeGameState(defaultGameState);

        return engine->Run();
    }
    else
    {
        LOG_ERROR("Could not create engine!");
        return -1;
    }
};
