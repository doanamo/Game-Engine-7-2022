/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include <doctest/doctest.h>
#include <Core/Core.hpp>
#include <Core/ReflectionGenerated.hpp>
#include <Game/ReflectionGenerated.hpp>
#include <Game/GameInstance.hpp>
#include <Game/EntitySystem.hpp>
#include <Game/Systems/IdentitySystem.hpp>

TEST_CASE("Identity System")
{
    Reflection::Generated::RegisterModuleCore();
    Reflection::Generated::RegisterModuleGame();

    std::unique_ptr<Game::GameInstance> gameInstance;
    gameInstance = Game::GameInstance::Create().UnwrapOr(nullptr);
    REQUIRE(gameInstance);

    Game::EntitySystem* entitySystem = gameInstance->GetSystem<Game::EntitySystem>();
    REQUIRE(entitySystem);

    Game::IdentitySystem* identitySystem = gameInstance->GetSystem<Game::IdentitySystem>();
    REQUIRE(identitySystem);

    CHECK_EQ(identitySystem->GetNamedEntityCount(), 0);

    SUBCASE("Names")
    {
        // Name invalid entity.
        CHECK_FALSE(identitySystem->SetEntityName(Game::EntityHandle(), "Invalid"));
        CHECK_FALSE(identitySystem->GetEntityByName("Invalid").IsSuccess());

        // Name single entity before processing entity commands.
        Game::EntityHandle entityPlayerOne = entitySystem->CreateEntity();
        CHECK(identitySystem->SetEntityName(entityPlayerOne, "PlayerOne"));
        CHECK_EQ(identitySystem->GetNamedEntityCount(), 1);

        entitySystem->ProcessCommands();
        CHECK(entityPlayerOne.IsValid());
        CHECK(entitySystem->IsEntityValid(entityPlayerOne));

        {
            auto namedEntityResult = identitySystem->GetEntityByName("PlayerOne");
            CHECK_EQ(namedEntityResult.Unwrap(), entityPlayerOne);
        }

        // Name single entity after processing entity commands.
        Game::EntityHandle entityPlayerTwo = entitySystem->CreateEntity();
        entitySystem->ProcessCommands();

        CHECK(entityPlayerTwo.IsValid());
        CHECK(entitySystem->IsEntityValid(entityPlayerTwo));
        CHECK(identitySystem->SetEntityName(entityPlayerTwo, "PlayerTwo"));

        {
            auto namedEntityResult = identitySystem->GetEntityByName("PlayerTwo");
            CHECK_EQ(namedEntityResult.Unwrap(), entityPlayerTwo);
        }

        // Query entity name.
        CHECK_EQ(identitySystem->GetEntityName(entityPlayerOne).Unwrap(), "PlayerOne");
        CHECK_EQ(identitySystem->GetEntityName(entityPlayerTwo).Unwrap(), "PlayerTwo");
        CHECK_EQ(identitySystem->GetNamedEntityCount(), 2);

        // Remove entity name by setting empty string.
        CHECK(identitySystem->SetEntityName(entityPlayerOne, ""));
        CHECK(identitySystem->SetEntityName(entityPlayerOne, ""));
        CHECK_FALSE(identitySystem->GetEntityByName("").IsSuccess());
        CHECK_FALSE(identitySystem->GetEntityName(entityPlayerOne).IsSuccess());

        // Rename entity.
        CHECK(identitySystem->SetEntityName(entityPlayerOne, "PlayerOne"));
        CHECK_FALSE(identitySystem->SetEntityName(entityPlayerOne, "PlayerTwo"));
        CHECK_EQ(identitySystem->GetEntityName(entityPlayerOne).Unwrap(), "PlayerOne");
        CHECK_NE(identitySystem->GetEntityName(entityPlayerOne).Unwrap(), "PlayerTwo");

        CHECK(identitySystem->SetEntityName(entityPlayerOne, "PlayerOnee"));
        CHECK_EQ(identitySystem->GetEntityName(entityPlayerOne).Unwrap(), "PlayerOnee");
        CHECK(identitySystem->SetEntityName(entityPlayerOne, "PlayerOne"));
        CHECK_EQ(identitySystem->GetEntityName(entityPlayerOne).Unwrap(), "PlayerOne");

        // Setting same name.
        CHECK(identitySystem->SetEntityName(entityPlayerOne, "PlayerOne"));
        CHECK(identitySystem->SetEntityName(entityPlayerTwo, "PlayerTwo"));

        // Name uniqueness.
        CHECK(identitySystem->SetEntityName(entityPlayerTwo, "PlayerOne", true));
        CHECK_EQ(identitySystem->GetEntityName(entityPlayerTwo).Unwrap(), "PlayerOne");
        CHECK_FALSE(identitySystem->GetEntityName(entityPlayerOne).IsSuccess());

        CHECK(identitySystem->SetEntityName(entityPlayerOne, "PlayerOne", true));
        CHECK_EQ(identitySystem->GetEntityName(entityPlayerOne).Unwrap(), "PlayerOne");
        CHECK(identitySystem->SetEntityName(entityPlayerTwo, "PlayerTwo"));
        CHECK_EQ(identitySystem->GetEntityName(entityPlayerTwo).Unwrap(), "PlayerTwo");

        // Query destroyed entity.
        entitySystem->DestroyAllEntities();
        CHECK_FALSE(identitySystem->GetEntityName(entityPlayerOne).IsSuccess());
        CHECK_FALSE(identitySystem->GetEntityByName("PlayerTwo").IsSuccess());
    }

    SUBCASE("Groups")
    {
        // Group invalid entity.
        CHECK_FALSE(identitySystem->SetEntityGroup(Game::EntityHandle(), "Invalid"));
        CHECK_FALSE(identitySystem->GetEntitiesByGroup("Invalid").IsSuccess());

        // Group single entity before processing entity commands.
        Game::EntityHandle entityPlayerOne = entitySystem->CreateEntity();
        CHECK(identitySystem->SetEntityGroup(entityPlayerOne, "GroupA"));
        CHECK_EQ(identitySystem->GetGroupedEntityCount(), 1);
        CHECK_EQ(identitySystem->GetGroupCount(), 1);

        entitySystem->ProcessCommands();
        CHECK(entityPlayerOne.IsValid());
        CHECK(entitySystem->IsEntityValid(entityPlayerOne));

        {
            auto groupedEntitiesResult = 
                identitySystem->GetEntitiesByGroup("GroupA").UnwrapSuccess();
            CHECK_NE(groupedEntitiesResult.find(entityPlayerOne), groupedEntitiesResult.end());
            CHECK_EQ(groupedEntitiesResult.size(), 1);
        }

        // Group multiple entities after processing entity commands.
        Game::EntityHandle entityPlayerTwo = entitySystem->CreateEntity();
        Game::EntityHandle entityPlayerThree = entitySystem->CreateEntity();
        Game::EntityHandle entityPlayerFour = entitySystem->CreateEntity();
        entitySystem->ProcessCommands();

        CHECK(identitySystem->SetEntityGroup(entityPlayerTwo, "GroupA"));
        CHECK(identitySystem->SetEntityGroup(entityPlayerThree, "GroupB"));
        CHECK(identitySystem->SetEntityGroup(entityPlayerFour, "GroupB"));

        {
            auto groupedEntitiesResultA = identitySystem->GetEntitiesByGroup("GroupA").Unwrap();
            CHECK_NE(groupedEntitiesResultA.find(entityPlayerOne), groupedEntitiesResultA.end());
            CHECK_NE(groupedEntitiesResultA.find(entityPlayerTwo), groupedEntitiesResultA.end());
            CHECK_EQ(groupedEntitiesResultA.size(), 2);

            auto groupedEntitiesResultB = identitySystem->GetEntitiesByGroup("GroupB").Unwrap();
            CHECK_NE(groupedEntitiesResultB.find(entityPlayerThree), groupedEntitiesResultB.end());
            CHECK_NE(groupedEntitiesResultB.find(entityPlayerFour), groupedEntitiesResultB.end());
            CHECK_EQ(groupedEntitiesResultB.size(), 2);
        }

        // Query entity group.
        CHECK(identitySystem->IsEntityInGroup(entityPlayerOne, "GroupA"));
        CHECK(identitySystem->IsEntityInGroup(entityPlayerThree, "GroupB"));
        CHECK_EQ(identitySystem->GetGroupedEntityCount(), 4);
        CHECK_EQ(identitySystem->GetGroupCount(), 2);

        // Setting same group.
        CHECK(identitySystem->SetEntityGroup(entityPlayerTwo, "GroupA"));
        CHECK(identitySystem->SetEntityGroup(entityPlayerFour, "GroupB"));

        // Query destroyed entity.
        entitySystem->DestroyAllEntities();
        CHECK_EQ(identitySystem->GetEntitiesByGroup("GroupA").UnwrapFailure(),
            Game::IdentitySystem::LookupErrors::GroupNotFound);
    }

    CHECK_EQ(identitySystem->GetNamedEntityCount(), 0);
    CHECK_EQ(identitySystem->GetGroupedEntityCount(), 0);
}
