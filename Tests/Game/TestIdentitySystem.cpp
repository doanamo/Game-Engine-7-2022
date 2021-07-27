/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#define DOCTEST_CONFIG_NO_SHORT_MACRO_NAMES
#include <doctest/doctest.h>

#include <Core/Core.hpp>
#include <Core/ReflectionGenerated.hpp>
#include <Game/ReflectionGenerated.hpp>
#include <Game/GameInstance.hpp>
#include <Game/EntitySystem.hpp>
#include <Game/Systems/IdentitySystem.hpp>

DOCTEST_TEST_CASE("Identity System")
{
    std::unique_ptr<Game::GameInstance> gameInstance;
    gameInstance = Game::GameInstance::Create().UnwrapOr(nullptr);
    DOCTEST_REQUIRE(gameInstance);

    Game::EntitySystem* entitySystem =
        gameInstance->GetSystems().Locate<Game::EntitySystem>();
    DOCTEST_REQUIRE(entitySystem);

    Game::IdentitySystem* identitySystem =
        gameInstance->GetSystems().Locate<Game::IdentitySystem>();
    DOCTEST_REQUIRE(identitySystem);

    DOCTEST_CHECK_EQ(identitySystem->GetNamedEntityCount(), 0);

    DOCTEST_SUBCASE("Names")
    {
        // Name invalid entity.
        DOCTEST_CHECK_FALSE(identitySystem->SetEntityName(Game::EntityHandle(), "Invalid"));
        DOCTEST_CHECK_FALSE(identitySystem->GetEntityByName("Invalid").IsSuccess());

        // Name single entity before processing entity commands.
        Game::EntityHandle entityPlayerOne = entitySystem->CreateEntity().Unwrap();
        DOCTEST_CHECK(identitySystem->SetEntityName(entityPlayerOne, "PlayerOne"));
        DOCTEST_CHECK_EQ(identitySystem->GetNamedEntityCount(), 1);

        entitySystem->ProcessCommands();
        DOCTEST_CHECK(entityPlayerOne.IsValid());
        DOCTEST_CHECK(entitySystem->IsEntityValid(entityPlayerOne));

        {
            auto namedEntityResult = identitySystem->GetEntityByName("PlayerOne");
            DOCTEST_CHECK_EQ(namedEntityResult.Unwrap(), entityPlayerOne);
        }

        // Name single entity after processing entity commands.
        Game::EntityHandle entityPlayerTwo = entitySystem->CreateEntity().Unwrap();
        entitySystem->ProcessCommands();

        DOCTEST_CHECK(entityPlayerTwo.IsValid());
        DOCTEST_CHECK(entitySystem->IsEntityValid(entityPlayerTwo));
        DOCTEST_CHECK(identitySystem->SetEntityName(entityPlayerTwo, "PlayerTwo"));

        {
            auto namedEntityResult = identitySystem->GetEntityByName("PlayerTwo");
            DOCTEST_CHECK_EQ(namedEntityResult.Unwrap(), entityPlayerTwo);
        }

        // Query entity name.
        DOCTEST_CHECK_EQ(identitySystem->GetEntityName(entityPlayerOne).Unwrap(), "PlayerOne");
        DOCTEST_CHECK_EQ(identitySystem->GetEntityName(entityPlayerTwo).Unwrap(), "PlayerTwo");
        DOCTEST_CHECK_EQ(identitySystem->GetNamedEntityCount(), 2);

        // Remove entity name by setting empty string.
        DOCTEST_CHECK(identitySystem->SetEntityName(entityPlayerOne, ""));
        DOCTEST_CHECK(identitySystem->SetEntityName(entityPlayerOne, ""));
        DOCTEST_CHECK_FALSE(identitySystem->GetEntityByName("").IsSuccess());
        DOCTEST_CHECK_FALSE(identitySystem->GetEntityName(entityPlayerOne).IsSuccess());

        // Rename entity.
        DOCTEST_CHECK(identitySystem->SetEntityName(entityPlayerOne, "PlayerOne"));
        DOCTEST_CHECK_FALSE(identitySystem->SetEntityName(entityPlayerOne, "PlayerTwo"));
        DOCTEST_CHECK_EQ(identitySystem->GetEntityName(entityPlayerOne).Unwrap(), "PlayerOne");
        DOCTEST_CHECK_NE(identitySystem->GetEntityName(entityPlayerOne).Unwrap(), "PlayerTwo");

        DOCTEST_CHECK(identitySystem->SetEntityName(entityPlayerOne, "PlayerOnee"));
        DOCTEST_CHECK_EQ(identitySystem->GetEntityName(entityPlayerOne).Unwrap(), "PlayerOnee");
        DOCTEST_CHECK(identitySystem->SetEntityName(entityPlayerOne, "PlayerOne"));
        DOCTEST_CHECK_EQ(identitySystem->GetEntityName(entityPlayerOne).Unwrap(), "PlayerOne");

        // Setting same name.
        DOCTEST_CHECK(identitySystem->SetEntityName(entityPlayerOne, "PlayerOne"));
        DOCTEST_CHECK(identitySystem->SetEntityName(entityPlayerTwo, "PlayerTwo"));

        // Name uniqueness.
        DOCTEST_CHECK(identitySystem->SetEntityName(entityPlayerTwo, "PlayerOne", true));
        DOCTEST_CHECK_EQ(identitySystem->GetEntityName(entityPlayerTwo).Unwrap(), "PlayerOne");
        DOCTEST_CHECK_FALSE(identitySystem->GetEntityName(entityPlayerOne).IsSuccess());

        DOCTEST_CHECK(identitySystem->SetEntityName(entityPlayerOne, "PlayerOne", true));
        DOCTEST_CHECK_EQ(identitySystem->GetEntityName(entityPlayerOne).Unwrap(), "PlayerOne");
        DOCTEST_CHECK(identitySystem->SetEntityName(entityPlayerTwo, "PlayerTwo"));
        DOCTEST_CHECK_EQ(identitySystem->GetEntityName(entityPlayerTwo).Unwrap(), "PlayerTwo");

        // Query destroyed entity.
        entitySystem->DestroyAllEntities();
        DOCTEST_CHECK_FALSE(identitySystem->GetEntityName(entityPlayerOne).IsSuccess());
        DOCTEST_CHECK_FALSE(identitySystem->GetEntityByName("PlayerTwo").IsSuccess());
    }

    DOCTEST_SUBCASE("Groups")
    {
        // Group invalid entity.
        DOCTEST_CHECK_FALSE(identitySystem->SetEntityGroup(Game::EntityHandle(), "Invalid"));
        DOCTEST_CHECK_FALSE(identitySystem->GetEntitiesByGroup("Invalid").IsSuccess());

        // DOCTEST_CHECK group of never registered entity.
        Game::EntityHandle entityPlayerNone = entitySystem->CreateEntity().Unwrap();
        DOCTEST_CHECK_EQ(identitySystem->GetEntityGroups(entityPlayerNone).UnwrapFailure(),
            Game::IdentitySystem::LookupErrors::EntityNotFound);

        // Group single entity before processing entity commands.
        Game::EntityHandle entityPlayerOne = entitySystem->CreateEntity().Unwrap();
        DOCTEST_CHECK(identitySystem->SetEntityGroup(entityPlayerOne, "GroupA"));
        DOCTEST_CHECK_EQ(identitySystem->GetGroupedEntityCount(), 1);
        DOCTEST_CHECK_EQ(identitySystem->GetGroupCount(), 1);

        entitySystem->ProcessCommands();
        DOCTEST_CHECK(entityPlayerOne.IsValid());
        DOCTEST_CHECK(entitySystem->IsEntityValid(entityPlayerOne));

        {
            auto groupedEntitiesResult = 
                identitySystem->GetEntitiesByGroup("GroupA").UnwrapSuccess();
            DOCTEST_CHECK_NE(groupedEntitiesResult.find(entityPlayerOne),
                groupedEntitiesResult.end());
            DOCTEST_CHECK_EQ(groupedEntitiesResult.size(), 1);
        }

        // Group multiple entities after processing entity commands.
        Game::EntityHandle entityPlayerTwo = entitySystem->CreateEntity().Unwrap();
        Game::EntityHandle entityPlayerThree = entitySystem->CreateEntity().Unwrap();
        Game::EntityHandle entityPlayerFour = entitySystem->CreateEntity().Unwrap();
        entitySystem->ProcessCommands();

        DOCTEST_CHECK(identitySystem->SetEntityGroup(entityPlayerTwo, "GroupA"));
        DOCTEST_CHECK(identitySystem->SetEntityGroup(entityPlayerThree, "GroupB"));
        DOCTEST_CHECK(identitySystem->SetEntityGroup(entityPlayerFour, "GroupB"));

        {
            auto groupedEntitiesResultA = identitySystem->GetEntitiesByGroup("GroupA").Unwrap();
            DOCTEST_CHECK_NE(groupedEntitiesResultA.find(entityPlayerOne),
                groupedEntitiesResultA.end());
            DOCTEST_CHECK_NE(groupedEntitiesResultA.find(entityPlayerTwo),
                groupedEntitiesResultA.end());
            DOCTEST_CHECK_EQ(groupedEntitiesResultA.size(), 2);

            auto groupedEntitiesResultB = identitySystem->GetEntitiesByGroup("GroupB").Unwrap();
            DOCTEST_CHECK_NE(groupedEntitiesResultB.find(entityPlayerThree),
                groupedEntitiesResultB.end());
            DOCTEST_CHECK_NE(groupedEntitiesResultB.find(entityPlayerFour),
                groupedEntitiesResultB.end());
            DOCTEST_CHECK_EQ(groupedEntitiesResultB.size(), 2);
        }

        // Query entity group.
        DOCTEST_CHECK(identitySystem->IsEntityInGroup(entityPlayerOne, "GroupA"));
        DOCTEST_CHECK(identitySystem->IsEntityInGroup(entityPlayerThree, "GroupB"));
        DOCTEST_CHECK_EQ(identitySystem->GetGroupedEntityCount(), 4);
        DOCTEST_CHECK_EQ(identitySystem->GetGroupCount(), 2);

        // Setting same group.
        DOCTEST_CHECK(identitySystem->SetEntityGroup(entityPlayerTwo, "GroupA"));
        DOCTEST_CHECK(identitySystem->SetEntityGroup(entityPlayerFour, "GroupB"));

        // Entity to multiple groups.
        DOCTEST_CHECK(identitySystem->IsEntityInGroup(entityPlayerOne, "GroupA"));
        DOCTEST_CHECK_FALSE(identitySystem->IsEntityInGroup(entityPlayerOne, "GroupB"));

        DOCTEST_CHECK(identitySystem->SetEntityGroup(entityPlayerOne, "GroupB"));
        DOCTEST_CHECK(identitySystem->IsEntityInGroup(entityPlayerOne, "GroupA"));
        DOCTEST_CHECK(identitySystem->IsEntityInGroup(entityPlayerOne, "GroupB"));

        // Clear group from entity.
        DOCTEST_CHECK_EQ(identitySystem->GetEntityGroupCount(entityPlayerOne), 2);
        DOCTEST_CHECK(identitySystem->ClearEntityGroup(entityPlayerOne, "GroupA"));
        DOCTEST_CHECK(identitySystem->ClearEntityGroup(entityPlayerOne, "GroupB"));
        DOCTEST_CHECK_EQ(identitySystem->GetEntityGroupCount(entityPlayerOne), 0);

        // Query destroyed entity.
        entitySystem->DestroyAllEntities();
        DOCTEST_CHECK_EQ(identitySystem->GetEntityGroups(entityPlayerThree).UnwrapFailure(),
            Game::IdentitySystem::LookupErrors::InvalidEntity);
        DOCTEST_CHECK_EQ(identitySystem->GetEntitiesByGroup("GroupA").UnwrapFailure(),
            Game::IdentitySystem::LookupErrors::GroupNotFound);
    }

    DOCTEST_CHECK_EQ(identitySystem->GetNamedEntityCount(), 0);
    DOCTEST_CHECK_EQ(identitySystem->GetGroupedEntityCount(), 0);
}
