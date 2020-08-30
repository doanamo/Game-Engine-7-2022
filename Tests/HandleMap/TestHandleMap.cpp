/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include <TestHelpers.hpp>
#include <Common/HandleMap.hpp>

int TestHandle()
{
    // Check invalid handles.
    struct Empty
    {
    };

    Common::Handle<Empty> handleA;
    TEST_FALSE(handleA.IsValid());
    TEST_EQ(handleA.GetIdentifier(), 0);
    TEST_EQ(handleA.GetVersion(), 0);

    Common::Handle<Empty> handleB;
    TEST_TRUE(handleA == handleB);
    TEST_TRUE(handleB == handleA);
    TEST_FALSE(handleA != handleB);
    TEST_FALSE(handleB != handleA);
    TEST_FALSE(handleA < handleB);
    TEST_FALSE(handleB < handleA);

    return true;
}

int TestMap(const int cacheSize)
{
    // Create empty handle map.
    struct Entity
    {
        std::string name;
    };

    Common::HandleMap<Entity> entities(cacheSize);
    TEST_EQ(entities.GetValidHandleCount(), 0);
    TEST_EQ(entities.GetUnusedHandleCount(), 0);

    TEST_FALSE(entities.DestroyHandle(Common::Handle<Entity>()));

    // Check empty handle map.
    {
        TEST_FALSE(entities.LookupHandle(Common::Handle<Entity>()).IsSuccess());
    }

    // Check single handle (with cache).
    {
        auto entityEntry = entities.CreateHandle().Unwrap();
        TEST_TRUE(entityEntry.storage != nullptr);
        TEST_TRUE(entityEntry.storage->name.empty());
        entityEntry.storage->name = "Entity";

        TEST_TRUE(entityEntry.handle.IsValid());
        TEST_EQ(entityEntry.handle.GetIdentifier(), 1);
        TEST_EQ(entityEntry.handle.GetVersion(), 0);

        TEST_EQ(entities.GetValidHandleCount(), 1);
        TEST_EQ(entities.GetUnusedHandleCount(), cacheSize);

        auto fetchedEntry = entities.LookupHandle(entityEntry.handle).Unwrap();
        TEST_EQ(fetchedEntry.handle, entityEntry.handle);
        TEST_EQ(fetchedEntry.storage, entityEntry.storage);
        TEST_EQ(fetchedEntry.storage->name, entityEntry.storage->name);

        TEST_TRUE(entities.DestroyHandle(entityEntry.handle));

        TEST_EQ(entities.GetValidHandleCount(), 0);
        TEST_EQ(entities.GetUnusedHandleCount(), cacheSize + 1);

        auto invalidatedEntry = entities.LookupHandle(entityEntry.handle);
        TEST_FALSE(invalidatedEntry.IsSuccess());

        TEST_FALSE(entities.DestroyHandle(entityEntry.handle));

        TEST_EQ(entities.GetValidHandleCount(), 0);
        TEST_EQ(entities.GetUnusedHandleCount(), cacheSize + 1);
    }

    // Checks subsequent handle.
    {
        auto entityEntry = entities.CreateHandle().Unwrap();
        TEST_TRUE(entityEntry.storage != nullptr);
        TEST_TRUE(entityEntry.storage->name.empty());

        TEST_TRUE(entityEntry.handle.IsValid());

        TEST_EQ(entities.GetValidHandleCount(), 1);
        TEST_EQ(entities.GetUnusedHandleCount(), cacheSize);

        TEST_TRUE(entities.DestroyHandle(entityEntry.handle));

        TEST_EQ(entities.GetValidHandleCount(), 0);
        TEST_EQ(entities.GetUnusedHandleCount(), cacheSize + 1);
    }

    // Check multiple handles.
    {
        auto entityEntryA = entities.CreateHandle().Unwrap();
        entityEntryA.storage->name = "EntityA";

        auto entityEntryB = entities.CreateHandle().Unwrap();
        entityEntryB.storage->name = "EntityB";

        auto entityEntryC = entities.CreateHandle().Unwrap();
        entityEntryC.storage->name = "EntityC";

        TEST_EQ(entities.GetValidHandleCount(), 3);
        TEST_EQ(entities.GetUnusedHandleCount(), cacheSize);

        TEST_TRUE(entities.DestroyHandle(entityEntryB.handle));

        TEST_EQ(entities.GetValidHandleCount(), 2);
        TEST_EQ(entities.GetUnusedHandleCount(), cacheSize + 1);

        TEST_TRUE(entities.DestroyHandle(entityEntryA.handle));

        TEST_EQ(entities.GetValidHandleCount(), 1);
        TEST_EQ(entities.GetUnusedHandleCount(), cacheSize + 2);

        TEST_TRUE(entities.DestroyHandle(entityEntryC.handle));

        TEST_EQ(entities.GetValidHandleCount(), 0);
        TEST_EQ(entities.GetUnusedHandleCount(), cacheSize + 3);

        auto recreatedEntityEntryA = entities.CreateHandle().Unwrap();
        TEST_TRUE(recreatedEntityEntryA.storage->name.empty());
        recreatedEntityEntryA.storage->name = "EntityA";

        auto recreatedEntityEntryB = entities.CreateHandle().Unwrap();
        TEST_TRUE(recreatedEntityEntryB.storage->name.empty());
        recreatedEntityEntryB.storage->name = "EntityB";

        auto recreatedEntityEntryC = entities.CreateHandle().Unwrap();
        TEST_TRUE(recreatedEntityEntryC.storage->name.empty());
        recreatedEntityEntryC.storage->name = "EntityC";

        auto fetchedEntityEntryA = entities.LookupHandle(recreatedEntityEntryA.handle).Unwrap();
        TEST_EQ(fetchedEntityEntryA.storage->name, "EntityA");

        auto fetchedEntityEntryB = entities.LookupHandle(recreatedEntityEntryB.handle).Unwrap();
        TEST_EQ(fetchedEntityEntryB.storage->name, "EntityB");

        auto fetchedEntityEntryC = entities.LookupHandle(recreatedEntityEntryC.handle).Unwrap();
        TEST_EQ(fetchedEntityEntryC.storage->name, "EntityC");

        // Checks creating requested handles.
        {
            Common::HandleMap<Entity> mirroredEntities(cacheSize);

            auto newEntityEntryB = mirroredEntities.CreateHandle(entityEntryB.handle).Unwrap();
            TEST_TRUE(newEntityEntryB.storage->name.empty());
            TEST_EQ(newEntityEntryB.handle.GetIdentifier(), entityEntryB.handle.GetIdentifier());
            TEST_EQ(newEntityEntryB.handle.GetVersion(), entityEntryB.handle.GetVersion());
            newEntityEntryB.storage->name = "EntityB";

            TEST_EQ(mirroredEntities.GetValidHandleCount(), 1);

            auto newEntityEntryC = mirroredEntities.CreateHandle(entityEntryC.handle).Unwrap();
            TEST_TRUE(newEntityEntryC.storage->name.empty());
            TEST_EQ(newEntityEntryC.handle.GetIdentifier(), entityEntryC.handle.GetIdentifier());
            TEST_EQ(newEntityEntryC.handle.GetVersion(), entityEntryC.handle.GetVersion());
            newEntityEntryC.storage->name = "EntityC";

            TEST_EQ(mirroredEntities.GetValidHandleCount(), 2);

            auto newEntityEntryA = mirroredEntities.CreateHandle(entityEntryA.handle).Unwrap();
            TEST_TRUE(newEntityEntryA.storage->name.empty());
            TEST_EQ(newEntityEntryA.handle.GetIdentifier(), entityEntryA.handle.GetIdentifier());
            TEST_EQ(newEntityEntryA.handle.GetVersion(), entityEntryA.handle.GetVersion());
            newEntityEntryA.storage->name = "EntityA";

            TEST_EQ(mirroredEntities.GetValidHandleCount(), 3);
        }
    }

    return true;
}

int TestIterator()
{
    // Create populated handle map.
    struct Entity
    {
        int counter = 0;
    };

    Common::Handle<Entity> entityHandles[10];

    Common::HandleMap<Entity> entities;
    TEST_EQ(entities.GetValidHandleCount(), 0);
    TEST_EQ(entities.GetUnusedHandleCount(), 0);

    for(int i = 0; i < 10; ++i)
    {
        auto entityEntry = entities.CreateHandle().Unwrap();
        entityEntry.storage->counter = i;
        entityHandles[i] = entityEntry.handle;
    }

    TEST_TRUE(entities.DestroyHandle(entityHandles[0]));
    TEST_TRUE(entities.DestroyHandle(entityHandles[1]));
    TEST_TRUE(entities.DestroyHandle(entityHandles[3]));
    TEST_TRUE(entities.DestroyHandle(entityHandles[5]));
    TEST_TRUE(entities.DestroyHandle(entityHandles[9]));

    // Check entries using iterator.
    std::vector<Common::Handle<Entity>> validHandles;

    for(auto entityEntry : entities)
    {
        validHandles.push_back(entityEntry.handle);
    }

    TEST_EQ(validHandles.size(), 5);
    TEST_EQ(entities.LookupHandle(validHandles[0]).Unwrap().handle.GetIdentifier(), 3);
    TEST_EQ(entities.LookupHandle(validHandles[1]).Unwrap().handle.GetIdentifier(), 5);
    TEST_EQ(entities.LookupHandle(validHandles[2]).Unwrap().handle.GetIdentifier(), 7);
    TEST_EQ(entities.LookupHandle(validHandles[3]).Unwrap().handle.GetIdentifier(), 8);
    TEST_EQ(entities.LookupHandle(validHandles[4]).Unwrap().handle.GetIdentifier(), 9);

    std::vector<Common::Handle<Entity>> constValidHandles;

    const Common::HandleMap<Entity>& constEntities = entities;

    for(auto entityEntry : constEntities)
    {
        constValidHandles.push_back(entityEntry.handle);
    }

    TEST_EQ(constValidHandles.size(), 5);
    TEST_EQ(entities.LookupHandle(constValidHandles[0]).Unwrap().handle.GetIdentifier(), 3);
    TEST_EQ(entities.LookupHandle(constValidHandles[1]).Unwrap().handle.GetIdentifier(), 5);
    TEST_EQ(entities.LookupHandle(constValidHandles[2]).Unwrap().handle.GetIdentifier(), 7);
    TEST_EQ(entities.LookupHandle(constValidHandles[3]).Unwrap().handle.GetIdentifier(), 8);
    TEST_EQ(entities.LookupHandle(constValidHandles[4]).Unwrap().handle.GetIdentifier(), 9);

    return true;
}

int main()
{
    TEST_RUN(TestHandle);
    TEST_RUN(TestMap, 0);
    TEST_RUN(TestMap, 1);
    TEST_RUN(TestMap, 2);
    TEST_RUN(TestMap, 3);
    TEST_RUN(TestMap, 4);
    TEST_RUN(TestMap, 32);
    TEST_RUN(TestIterator);
}
