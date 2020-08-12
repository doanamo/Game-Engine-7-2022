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

int TestMap()
{
    // Create empty handle map.
    struct Entity
    {
        std::string name;
    };

    Common::HandleMap<Entity> entities(32);
    TEST_EQ(entities.GetValidHandleCount(), 0);
    TEST_EQ(entities.GetUnusedHandleCount(), 0);

    // Check empty handle map.
    {
        auto entityEntry = entities.LookupHandle(Common::Handle<Entity>());
        TEST_EQ(entityEntry.handle, Common::Handle<Entity>());
        TEST_EQ(entityEntry.storage, nullptr);
        TEST_FALSE(entityEntry.valid);
    }

    TEST_FALSE(entities.DestroyHandle(Common::Handle<Entity>()));

    // Check single handle.
    {
        auto entityEntry = entities.CreateHandle();
        TEST_TRUE(entityEntry.storage != nullptr);
        TEST_TRUE(entityEntry.storage->name.empty());
        TEST_TRUE(entityEntry.valid);
        entityEntry.storage->name = "Entity";

        TEST_TRUE(entityEntry.handle.IsValid());
        TEST_EQ(entityEntry.handle.GetIdentifier(), 1);
        TEST_EQ(entityEntry.handle.GetVersion(), 0);

        TEST_EQ(entities.GetValidHandleCount(), 1);
        TEST_EQ(entities.GetUnusedHandleCount(), 32);

        auto fetchedEntry = entities.LookupHandle(entityEntry.handle);
        TEST_EQ(fetchedEntry.handle, entityEntry.handle);
        TEST_EQ(fetchedEntry.valid, entityEntry.valid);
        TEST_EQ(fetchedEntry.storage, entityEntry.storage);
        TEST_EQ(fetchedEntry.storage->name, entityEntry.storage->name);

        TEST_TRUE(entities.DestroyHandle(entityEntry.handle));

        TEST_EQ(entities.GetValidHandleCount(), 0);
        TEST_EQ(entities.GetUnusedHandleCount(), 33);

        auto invalidatedEntry = entities.LookupHandle(entityEntry.handle);
        TEST_EQ(invalidatedEntry.handle, Common::Handle<Entity>());
        TEST_EQ(invalidatedEntry.storage, nullptr);
        TEST_FALSE(invalidatedEntry.valid);

        TEST_FALSE(entities.DestroyHandle(entityEntry.handle));

        TEST_EQ(entities.GetValidHandleCount(), 0);
        TEST_EQ(entities.GetUnusedHandleCount(), 33);
    }

    // Checks subsequent handle.
    {
        auto entityEntry = entities.CreateHandle();
        TEST_TRUE(entityEntry.storage != nullptr);
        TEST_TRUE(entityEntry.storage->name.empty());
        TEST_TRUE(entityEntry.valid);

        TEST_TRUE(entityEntry.handle.IsValid());
        TEST_EQ(entityEntry.handle.GetIdentifier(), 2);
        TEST_EQ(entityEntry.handle.GetVersion(), 0);

        TEST_EQ(entities.GetValidHandleCount(), 1);
        TEST_EQ(entities.GetUnusedHandleCount(), 32);

        TEST_TRUE(entities.DestroyHandle(entityEntry.handle));

        TEST_EQ(entities.GetValidHandleCount(), 0);
        TEST_EQ(entities.GetUnusedHandleCount(), 33);
    }

    // Check multiple handles.
    {
        auto entityEntryA = entities.CreateHandle();
        entityEntryA.storage->name = "EntityA";

        auto entityEntryB = entities.CreateHandle();
        entityEntryB.storage->name = "EntityB";

        auto entityEntryC = entities.CreateHandle();
        entityEntryC.storage->name = "EntityC";

        TEST_EQ(entities.GetValidHandleCount(), 3);
        TEST_EQ(entities.GetUnusedHandleCount(), 32);

        TEST_TRUE(entities.DestroyHandle(entityEntryB.handle));

        TEST_EQ(entities.GetValidHandleCount(), 2);
        TEST_EQ(entities.GetUnusedHandleCount(), 33);

        TEST_TRUE(entities.DestroyHandle(entityEntryA.handle));

        TEST_EQ(entities.GetValidHandleCount(), 1);
        TEST_EQ(entities.GetUnusedHandleCount(), 34);

        TEST_TRUE(entities.DestroyHandle(entityEntryC.handle));

        TEST_EQ(entities.GetValidHandleCount(), 0);
        TEST_EQ(entities.GetUnusedHandleCount(), 35);

        auto recreatedEntityEntryA = entities.CreateHandle();
        TEST_TRUE(recreatedEntityEntryA.valid);
        TEST_TRUE(recreatedEntityEntryA.storage->name.empty());
        recreatedEntityEntryA.storage->name = "EntityA";

        auto recreatedEntityEntryB = entities.CreateHandle();
        TEST_TRUE(recreatedEntityEntryB.valid);
        TEST_TRUE(recreatedEntityEntryB.storage->name.empty());
        recreatedEntityEntryB.storage->name = "EntityB";

        auto recreatedEntityEntryC = entities.CreateHandle();
        TEST_TRUE(recreatedEntityEntryC.valid);
        TEST_TRUE(recreatedEntityEntryC.storage->name.empty());
        recreatedEntityEntryC.storage->name = "EntityC";

        auto fetchedEntityEntryA = entities.LookupHandle(recreatedEntityEntryA.handle);
        TEST_TRUE(recreatedEntityEntryA.valid);
        TEST_EQ(fetchedEntityEntryA.storage->name, "EntityA");

        auto fetchedEntityEntryB = entities.LookupHandle(recreatedEntityEntryB.handle);
        TEST_TRUE(recreatedEntityEntryB.valid);
        TEST_EQ(fetchedEntityEntryB.storage->name, "EntityB");

        auto fetchedEntityEntryC = entities.LookupHandle(recreatedEntityEntryC.handle);
        TEST_TRUE(recreatedEntityEntryC.valid);
        TEST_EQ(fetchedEntityEntryC.storage->name, "EntityC");

        // Checks creating requested handles.
        {
            Common::HandleMap<Entity> mirroredEntities;

            auto newEntityEntryB = mirroredEntities.CreateHandle(entityEntryB.handle);
            TEST_TRUE(newEntityEntryB.valid);

            TEST_TRUE(newEntityEntryB.storage->name.empty());
            newEntityEntryB.storage->name = "EntityB";

            TEST_EQ(mirroredEntities.GetValidHandleCount(), 1);
            TEST_EQ(mirroredEntities.GetUnusedHandleCount(), 3);

            auto newEntityEntryC = mirroredEntities.CreateHandle(entityEntryC.handle);
            TEST_TRUE(newEntityEntryC.valid);

            TEST_TRUE(newEntityEntryC.storage->name.empty());
            newEntityEntryC.storage->name = "EntityC";

            TEST_EQ(mirroredEntities.GetValidHandleCount(), 2);
            TEST_EQ(mirroredEntities.GetUnusedHandleCount(), 3);

            auto newEntityEntryA = mirroredEntities.CreateHandle(entityEntryA.handle);
            TEST_TRUE(newEntityEntryA.valid);

            TEST_TRUE(newEntityEntryA.storage->name.empty());
            newEntityEntryA.storage->name = "EntityA";

            TEST_EQ(mirroredEntities.GetValidHandleCount(), 3);
            TEST_EQ(mirroredEntities.GetUnusedHandleCount(), 2);
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
        auto entityEntry = entities.CreateHandle();
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
        TEST_TRUE(entityEntry.valid);
        validHandles.push_back(entityEntry.handle);
    }

    TEST_EQ(validHandles.size(), 5);
    TEST_EQ(entities.LookupHandle(validHandles[0]).handle.GetIdentifier(), 3);
    TEST_EQ(entities.LookupHandle(validHandles[1]).handle.GetIdentifier(), 5);
    TEST_EQ(entities.LookupHandle(validHandles[2]).handle.GetIdentifier(), 7);
    TEST_EQ(entities.LookupHandle(validHandles[3]).handle.GetIdentifier(), 8);
    TEST_EQ(entities.LookupHandle(validHandles[4]).handle.GetIdentifier(), 9);

    std::vector<Common::Handle<Entity>> constValidHandles;

    const Common::HandleMap<Entity>& constEntities = entities;

    for(auto entityEntry : constEntities)
    {
        TEST_TRUE(entityEntry.valid);
        constValidHandles.push_back(entityEntry.handle);
    }

    TEST_EQ(constValidHandles.size(), 5);
    TEST_EQ(entities.LookupHandle(constValidHandles[0]).handle.GetIdentifier(), 3);
    TEST_EQ(entities.LookupHandle(constValidHandles[1]).handle.GetIdentifier(), 5);
    TEST_EQ(entities.LookupHandle(constValidHandles[2]).handle.GetIdentifier(), 7);
    TEST_EQ(entities.LookupHandle(constValidHandles[3]).handle.GetIdentifier(), 8);
    TEST_EQ(entities.LookupHandle(constValidHandles[4]).handle.GetIdentifier(), 9);

    return true;
}

int main()
{
    TEST_RUN(TestHandle);
    TEST_RUN(TestMap);
    TEST_RUN(TestIterator);
}
