/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include <TestHelpers.hpp>
#include <Game/EntitySystem.hpp>
#include <Game/Systems/IdentitySystem.hpp>

bool TestNames()
{
    // Create identity system.
    std::unique_ptr<Game::EntitySystem> entitySystem;
    entitySystem = Game::EntitySystem::Create().Unwrap();

    Game::IdentitySystem::CreateFromParams params;
    params.entitySystem = entitySystem.get();

    std::unique_ptr<Game::IdentitySystem> identitySystem;
    identitySystem = Game::IdentitySystem::Create(params).Unwrap();
    TEST_EQ(identitySystem->GetNamedEntityCount(), 0);

    // Name invalid entity.
    TEST_FALSE(identitySystem->SetEntityName(Game::EntityHandle(), "Invalid"));
    TEST_FALSE(identitySystem->GetEntityByName("Invalid").IsSuccess());

    // Name single entity (before processing entity commands).
    Game::EntityHandle entityPlayerOne = entitySystem->CreateEntity();
    TEST_TRUE(identitySystem->SetEntityName(entityPlayerOne, "PlayerOne"));
    TEST_EQ(identitySystem->GetNamedEntityCount(), 1);

    entitySystem->ProcessCommands();
    TEST_TRUE(entityPlayerOne.IsValid());
    TEST_TRUE(entitySystem->IsEntityValid(entityPlayerOne));

    {
        auto namedEntityResult = identitySystem->GetEntityByName("PlayerOne");
        TEST_EQ(namedEntityResult.Unwrap(), entityPlayerOne);
    }

    // Name single entity (after processing entity commands).
    Game::EntityHandle entityPlayerTwo = entitySystem->CreateEntity();
    entitySystem->ProcessCommands();
    TEST_TRUE(entityPlayerTwo.IsValid());
    TEST_TRUE(entitySystem->IsEntityValid(entityPlayerTwo));
    TEST_TRUE(identitySystem->SetEntityName(entityPlayerTwo, "PlayerTwo"));

    {
        auto namedEntityResult = identitySystem->GetEntityByName("PlayerTwo");
        TEST_EQ(namedEntityResult.Unwrap(), entityPlayerTwo);
    }

    // Query entity name.
    TEST_EQ(identitySystem->GetEntityName(entityPlayerOne).Unwrap(), "PlayerOne");
    TEST_EQ(identitySystem->GetEntityName(entityPlayerTwo).Unwrap(), "PlayerTwo");
    TEST_EQ(identitySystem->GetNamedEntityCount(), 2);

    // Remove entity name by setting empty string.
    TEST_TRUE(identitySystem->SetEntityName(entityPlayerOne, ""));
    TEST_TRUE(identitySystem->SetEntityName(entityPlayerOne, ""));
    TEST_FALSE(identitySystem->GetEntityByName("").IsSuccess());
    TEST_FALSE(identitySystem->GetEntityName(entityPlayerOne).IsSuccess());

    // Rename entity.
    TEST_TRUE(identitySystem->SetEntityName(entityPlayerOne, "PlayerOne"));
    TEST_FALSE(identitySystem->SetEntityName(entityPlayerOne, "PlayerTwo"));
    TEST_EQ(identitySystem->GetEntityName(entityPlayerOne).Unwrap(), "PlayerOne");
    TEST_NEQ(identitySystem->GetEntityName(entityPlayerOne).Unwrap(), "PlayerTwo");

    TEST_TRUE(identitySystem->SetEntityName(entityPlayerOne, "PlayerOnee"));
    TEST_EQ(identitySystem->GetEntityName(entityPlayerOne).Unwrap(), "PlayerOnee");
    TEST_TRUE(identitySystem->SetEntityName(entityPlayerOne, "PlayerOne"));
    TEST_EQ(identitySystem->GetEntityName(entityPlayerOne).Unwrap(), "PlayerOne");

    // Setting same name.
    TEST_TRUE(identitySystem->SetEntityName(entityPlayerOne, "PlayerOne"));
    TEST_TRUE(identitySystem->SetEntityName(entityPlayerTwo, "PlayerTwo"));

    // Name uniqueness.
    TEST_TRUE(identitySystem->SetEntityName(entityPlayerTwo, "PlayerOne", true));
    TEST_EQ(identitySystem->GetEntityName(entityPlayerTwo).Unwrap(), "PlayerOne");
    TEST_FALSE(identitySystem->GetEntityName(entityPlayerOne).IsSuccess());

    TEST_TRUE(identitySystem->SetEntityName(entityPlayerOne, "PlayerOne", true));
    TEST_EQ(identitySystem->GetEntityName(entityPlayerOne).Unwrap(), "PlayerOne");
    TEST_TRUE(identitySystem->SetEntityName(entityPlayerTwo, "PlayerTwo"));
    TEST_EQ(identitySystem->GetEntityName(entityPlayerTwo).Unwrap(), "PlayerTwo");

    // Query destroyed entity.
    entitySystem->DestroyAllEntities();
    TEST_FALSE(identitySystem->GetEntityName(entityPlayerOne).IsSuccess());
    TEST_FALSE(identitySystem->GetEntityByName("PlayerTwo").IsSuccess());
    TEST_EQ(identitySystem->GetNamedEntityCount(), 0);

    return true;
}

bool TestGroups()
{
    // Create identity system.
    std::unique_ptr<Game::EntitySystem> entitySystem;
    entitySystem = Game::EntitySystem::Create().Unwrap();

    using Game::IdentitySystem;
    IdentitySystem::CreateFromParams params;
    params.entitySystem = entitySystem.get();

    std::unique_ptr<IdentitySystem> identitySystem;
    identitySystem = IdentitySystem::Create(params).Unwrap();
    TEST_EQ(identitySystem->GetNamedEntityCount(), 0);

    // Group invalid entity.
    TEST_FALSE(identitySystem->SetEntityGroup(Game::EntityHandle(), "Invalid"));
    TEST_FALSE(identitySystem->GetEntitiesByGroup("Invalid").IsSuccess());

    // Group single entity (before processing entity commands).
    Game::EntityHandle entityPlayerOne = entitySystem->CreateEntity();
    TEST_TRUE(identitySystem->SetEntityGroup(entityPlayerOne, "GroupA"));
    TEST_EQ(identitySystem->GetGroupedEntityCount(), 1);
    TEST_EQ(identitySystem->GetGroupCount(), 1);

    entitySystem->ProcessCommands();
    TEST_TRUE(entityPlayerOne.IsValid());
    TEST_TRUE(entitySystem->IsEntityValid(entityPlayerOne));

    {
        auto groupedEntitiesResult = identitySystem->GetEntitiesByGroup("GroupA").UnwrapSuccess();
        TEST_NEQ(groupedEntitiesResult.find(entityPlayerOne), groupedEntitiesResult.end());
        TEST_EQ(groupedEntitiesResult.size(), 1);
    }

    // Group multiple entities (after processing entity commands).
    Game::EntityHandle entityPlayerTwo = entitySystem->CreateEntity();
    Game::EntityHandle entityPlayerThree = entitySystem->CreateEntity();
    Game::EntityHandle entityPlayerFour = entitySystem->CreateEntity();
    entitySystem->ProcessCommands();

    TEST_TRUE(identitySystem->SetEntityGroup(entityPlayerTwo, "GroupA"));
    TEST_TRUE(identitySystem->SetEntityGroup(entityPlayerThree, "GroupB"));
    TEST_TRUE(identitySystem->SetEntityGroup(entityPlayerFour, "GroupB"));

    {
        auto groupedEntitiesResultA = identitySystem->GetEntitiesByGroup("GroupA").Unwrap();
        TEST_NEQ(groupedEntitiesResultA.find(entityPlayerOne), groupedEntitiesResultA.end());
        TEST_NEQ(groupedEntitiesResultA.find(entityPlayerTwo), groupedEntitiesResultA.end());
        TEST_EQ(groupedEntitiesResultA.size(), 2);

        auto groupedEntitiesResultB = identitySystem->GetEntitiesByGroup("GroupB").Unwrap();
        TEST_NEQ(groupedEntitiesResultB.find(entityPlayerThree), groupedEntitiesResultB.end());
        TEST_NEQ(groupedEntitiesResultB.find(entityPlayerFour), groupedEntitiesResultB.end());
        TEST_EQ(groupedEntitiesResultB.size(), 2);
    }

    // Query entity group.
    TEST_TRUE(identitySystem->IsEntityInGroup(entityPlayerOne, "GroupA"));
    TEST_TRUE(identitySystem->IsEntityInGroup(entityPlayerThree, "GroupB"));
    TEST_EQ(identitySystem->GetGroupedEntityCount(), 4);
    TEST_EQ(identitySystem->GetGroupCount(), 2);

    // Setting same group.
    TEST_TRUE(identitySystem->SetEntityGroup(entityPlayerTwo, "GroupA"));
    TEST_TRUE(identitySystem->SetEntityGroup(entityPlayerFour, "GroupB"));

    // Query destroyed entity.
    entitySystem->DestroyAllEntities();
    TEST_EQ(identitySystem->GetEntitiesByGroup("GroupA").UnwrapFailure(),
        IdentitySystem::LookupErrors::GroupNotFound);

    return true;
}

int main()
{
    TEST_RUN(TestNames);
    TEST_RUN(TestGroups);
}
