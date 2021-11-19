/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include <Core/Core.hpp>
#include <Core/ReflectionGenerated.hpp>
#include <Game/ReflectionGenerated.hpp>
#include <Game/GameInstance.hpp>
#include <Game/EntitySystem.hpp>
#include <Game/Systems/IdentitySystem.hpp>
#include <gtest/gtest.h>

/*
    Identity System
*/

class IdentitySystem : public testing::Test
{
protected:
    void SetUp() override
    {
        gameInstance = Game::GameInstance::Create().UnwrapOr(nullptr);
        ASSERT_TRUE(gameInstance);

        entitySystem = &gameInstance->GetSystems().Locate<Game::EntitySystem>();
        identitySystem = &gameInstance->GetSystems().Locate<Game::IdentitySystem>();
        ASSERT_EQ(identitySystem->GetNamedEntityCount(), 0);
    }

    void TearDown() override
    {
        if(identitySystem)
        {
            EXPECT_EQ(identitySystem->GetNamedEntityCount(), 0);
            EXPECT_EQ(identitySystem->GetGroupedEntityCount(), 0);
        }
    }

protected:
    std::unique_ptr<Game::GameInstance> gameInstance;
    Game::EntitySystem* entitySystem = nullptr;
    Game::IdentitySystem* identitySystem = nullptr;
};

TEST_F(IdentitySystem, Names)
{
    // Name invalid entity.
    EXPECT_FALSE(identitySystem->SetEntityName(Game::EntityHandle(), "Invalid"));
    EXPECT_FALSE(identitySystem->GetEntityByName("Invalid").IsSuccess());

    // Name single entity before processing entity commands.
    Game::EntityHandle entityPlayerOne = entitySystem->CreateEntity().Unwrap();
    EXPECT_TRUE(identitySystem->SetEntityName(entityPlayerOne, "PlayerOne"));
    EXPECT_EQ(identitySystem->GetNamedEntityCount(), 1);

    entitySystem->ProcessCommands();
    EXPECT_TRUE(entityPlayerOne.IsValid());
    EXPECT_TRUE(entitySystem->IsEntityValid(entityPlayerOne));

    {
        auto namedEntityResult = identitySystem->GetEntityByName("PlayerOne");
        EXPECT_EQ(namedEntityResult.Unwrap(), entityPlayerOne);
    }

    // Name single entity after processing entity commands.
    Game::EntityHandle entityPlayerTwo = entitySystem->CreateEntity().Unwrap();
    entitySystem->ProcessCommands();

    EXPECT_TRUE(entityPlayerTwo.IsValid());
    EXPECT_TRUE(entitySystem->IsEntityValid(entityPlayerTwo));
    EXPECT_TRUE(identitySystem->SetEntityName(entityPlayerTwo, "PlayerTwo"));

    {
        auto namedEntityResult = identitySystem->GetEntityByName("PlayerTwo");
        EXPECT_EQ(namedEntityResult.Unwrap(), entityPlayerTwo);
    }

    // Query entity name.
    EXPECT_EQ(identitySystem->GetEntityName(entityPlayerOne).Unwrap(), "PlayerOne");
    EXPECT_EQ(identitySystem->GetEntityName(entityPlayerTwo).Unwrap(), "PlayerTwo");
    EXPECT_EQ(identitySystem->GetNamedEntityCount(), 2);

    // Remove entity name by setting empty string.
    EXPECT_TRUE(identitySystem->SetEntityName(entityPlayerOne, ""));
    EXPECT_TRUE(identitySystem->SetEntityName(entityPlayerOne, ""));
    EXPECT_FALSE(identitySystem->GetEntityByName("").IsSuccess());
    EXPECT_FALSE(identitySystem->GetEntityName(entityPlayerOne).IsSuccess());

    // Rename entity.
    EXPECT_TRUE(identitySystem->SetEntityName(entityPlayerOne, "PlayerOne"));
    EXPECT_FALSE(identitySystem->SetEntityName(entityPlayerOne, "PlayerTwo"));
    EXPECT_EQ(identitySystem->GetEntityName(entityPlayerOne).Unwrap(), "PlayerOne");
    EXPECT_NE(identitySystem->GetEntityName(entityPlayerOne).Unwrap(), "PlayerTwo");

    EXPECT_TRUE(identitySystem->SetEntityName(entityPlayerOne, "PlayerOnee"));
    EXPECT_EQ(identitySystem->GetEntityName(entityPlayerOne).Unwrap(), "PlayerOnee");
    EXPECT_TRUE(identitySystem->SetEntityName(entityPlayerOne, "PlayerOne"));
    EXPECT_EQ(identitySystem->GetEntityName(entityPlayerOne).Unwrap(), "PlayerOne");

    // Setting same name.
    EXPECT_TRUE(identitySystem->SetEntityName(entityPlayerOne, "PlayerOne"));
    EXPECT_TRUE(identitySystem->SetEntityName(entityPlayerTwo, "PlayerTwo"));

    // Name uniqueness.
    EXPECT_TRUE(identitySystem->SetEntityName(entityPlayerTwo, "PlayerOne", true));
    EXPECT_EQ(identitySystem->GetEntityName(entityPlayerTwo).Unwrap(), "PlayerOne");
    EXPECT_FALSE(identitySystem->GetEntityName(entityPlayerOne).IsSuccess());

    EXPECT_TRUE(identitySystem->SetEntityName(entityPlayerOne, "PlayerOne", true));
    EXPECT_EQ(identitySystem->GetEntityName(entityPlayerOne).Unwrap(), "PlayerOne");
    EXPECT_TRUE(identitySystem->SetEntityName(entityPlayerTwo, "PlayerTwo"));
    EXPECT_EQ(identitySystem->GetEntityName(entityPlayerTwo).Unwrap(), "PlayerTwo");

    // Query destroyed entity.
    entitySystem->DestroyAllEntities();
    EXPECT_FALSE(identitySystem->GetEntityName(entityPlayerOne).IsSuccess());
    EXPECT_FALSE(identitySystem->GetEntityByName("PlayerTwo").IsSuccess());
}

TEST_F(IdentitySystem, Groups)
{
    // Group invalid entity.
    EXPECT_FALSE(identitySystem->SetEntityGroup(Game::EntityHandle(), "Invalid"));
    EXPECT_FALSE(identitySystem->GetEntitiesByGroup("Invalid").IsSuccess());

    // EXPECT_TRUE group of never registered entity.
    Game::EntityHandle entityPlayerNone = entitySystem->CreateEntity().Unwrap();
    EXPECT_EQ(identitySystem->GetEntityGroups(entityPlayerNone).UnwrapFailure(),
        Game::IdentitySystem::LookupErrors::EntityNotFound);

    // Group single entity before processing entity commands.
    Game::EntityHandle entityPlayerOne = entitySystem->CreateEntity().Unwrap();
    EXPECT_TRUE(identitySystem->SetEntityGroup(entityPlayerOne, "GroupA"));
    EXPECT_EQ(identitySystem->GetGroupedEntityCount(), 1);
    EXPECT_EQ(identitySystem->GetGroupCount(), 1);

    entitySystem->ProcessCommands();
    EXPECT_TRUE(entityPlayerOne.IsValid());
    EXPECT_TRUE(entitySystem->IsEntityValid(entityPlayerOne));

    {
        auto groupedEntitiesResult =
            identitySystem->GetEntitiesByGroup("GroupA").UnwrapSuccess();
        EXPECT_NE(groupedEntitiesResult.find(entityPlayerOne),
            groupedEntitiesResult.end());
        EXPECT_EQ(groupedEntitiesResult.size(), 1);
    }

    // Group multiple entities after processing entity commands.
    Game::EntityHandle entityPlayerTwo = entitySystem->CreateEntity().Unwrap();
    Game::EntityHandle entityPlayerThree = entitySystem->CreateEntity().Unwrap();
    Game::EntityHandle entityPlayerFour = entitySystem->CreateEntity().Unwrap();
    entitySystem->ProcessCommands();

    EXPECT_TRUE(identitySystem->SetEntityGroup(entityPlayerTwo, "GroupA"));
    EXPECT_TRUE(identitySystem->SetEntityGroup(entityPlayerThree, "GroupB"));
    EXPECT_TRUE(identitySystem->SetEntityGroup(entityPlayerFour, "GroupB"));

    {
        auto groupedEntitiesResultA = identitySystem->GetEntitiesByGroup("GroupA").Unwrap();
        EXPECT_NE(groupedEntitiesResultA.find(entityPlayerOne),
            groupedEntitiesResultA.end());
        EXPECT_NE(groupedEntitiesResultA.find(entityPlayerTwo),
            groupedEntitiesResultA.end());
        EXPECT_EQ(groupedEntitiesResultA.size(), 2);

        auto groupedEntitiesResultB = identitySystem->GetEntitiesByGroup("GroupB").Unwrap();
        EXPECT_NE(groupedEntitiesResultB.find(entityPlayerThree),
            groupedEntitiesResultB.end());
        EXPECT_NE(groupedEntitiesResultB.find(entityPlayerFour),
            groupedEntitiesResultB.end());
        EXPECT_EQ(groupedEntitiesResultB.size(), 2);
    }

    // Query entity group.
    EXPECT_TRUE(identitySystem->IsEntityInGroup(entityPlayerOne, "GroupA"));
    EXPECT_TRUE(identitySystem->IsEntityInGroup(entityPlayerThree, "GroupB"));
    EXPECT_EQ(identitySystem->GetGroupedEntityCount(), 4);
    EXPECT_EQ(identitySystem->GetGroupCount(), 2);

    // Setting same group.
    EXPECT_TRUE(identitySystem->SetEntityGroup(entityPlayerTwo, "GroupA"));
    EXPECT_TRUE(identitySystem->SetEntityGroup(entityPlayerFour, "GroupB"));

    // Entity to multiple groups.
    EXPECT_TRUE(identitySystem->IsEntityInGroup(entityPlayerOne, "GroupA"));
    EXPECT_FALSE(identitySystem->IsEntityInGroup(entityPlayerOne, "GroupB"));

    EXPECT_TRUE(identitySystem->SetEntityGroup(entityPlayerOne, "GroupB"));
    EXPECT_TRUE(identitySystem->IsEntityInGroup(entityPlayerOne, "GroupA"));
    EXPECT_TRUE(identitySystem->IsEntityInGroup(entityPlayerOne, "GroupB"));

    // Clear group from entity.
    EXPECT_EQ(identitySystem->GetEntityGroupCount(entityPlayerOne), 2);
    EXPECT_TRUE(identitySystem->ClearEntityGroup(entityPlayerOne, "GroupA"));
    EXPECT_TRUE(identitySystem->ClearEntityGroup(entityPlayerOne, "GroupB"));
    EXPECT_EQ(identitySystem->GetEntityGroupCount(entityPlayerOne), 0);

    // Query destroyed entity.
    entitySystem->DestroyAllEntities();
    EXPECT_EQ(identitySystem->GetEntityGroups(entityPlayerThree).UnwrapFailure(),
        Game::IdentitySystem::LookupErrors::InvalidEntity);
    EXPECT_EQ(identitySystem->GetEntitiesByGroup("GroupA").UnwrapFailure(),
        Game::IdentitySystem::LookupErrors::GroupNotFound);
}
