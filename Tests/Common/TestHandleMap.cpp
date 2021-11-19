/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include <random>
#include <fmt/core.h>
#include <Common/HandleMap.hpp>
#include <gtest/gtest.h>

/*
    Helpers
*/

struct Empty
{
};

struct Named
{
    std::string name;
};

struct Counter
{
    int counter = 0;
};

/*
    Handle
*/

TEST(Handle, Basic)
{
    Common::Handle<Empty> handleA;
    EXPECT_FALSE(handleA.IsValid());
    EXPECT_EQ(handleA.GetIdentifier(), 0);
    EXPECT_EQ(handleA.GetVersion(), 0);

    Common::Handle<Empty> handleB;
    EXPECT_EQ(handleA, handleB);
    EXPECT_EQ(handleB, handleA);
    EXPECT_FALSE(handleA != handleB);
    EXPECT_FALSE(handleB != handleA);
    EXPECT_FALSE(handleA < handleB);
    EXPECT_FALSE(handleB < handleA);
}

/*
    Handle Map
*/

TEST(HandleMap, Empty)
{
    for(int cacheSize : { 0, 1, 2, 3, 8 })
    {
        Common::HandleMap<Named> entities(cacheSize);

        EXPECT_EQ(entities.GetValidHandleCount(), 0);
        EXPECT_EQ(entities.GetUnusedHandleCount(), 0);
    }
}

TEST(HandleMap, InvalidHandle)
{
    for(int cacheSize : { 0, 1, 2, 3, 8 })
    {
        Common::HandleMap<Named> entities(cacheSize);

        EXPECT_FALSE(entities.LookupHandle(Common::Handle<Named>()));
        EXPECT_FALSE(entities.DestroyHandle(Common::Handle<Named>()));
    }
}

TEST(HandleMap, HandleCreationAndMirroring)
{
    for(int cacheSize : { 0, 1, 3, 8 })
    {
        const int createCount = 4;
        const int recreateCount = cacheSize + 1;

        for(int createIndex = 0; createIndex < createCount; ++createIndex)
        for(int recreateIndex = 0; recreateIndex <= recreateCount; ++recreateIndex)
        {
            SCOPED_TRACE(fmt::format("Create and mirror {} handle(s)", createIndex + 1));

            if(recreateIndex != 0)
            {
                SCOPED_TRACE(fmt::format("Recreate handle(s) (iteration #{})", recreateIndex));
            }

            Common::HandleMap<Named> entities(cacheSize);
            Common::HandleMap<Named> entitiesMirror(cacheSize);

            Common::HandleMap<Named>::HandleEntryRef entityEntries[createCount];
            Common::HandleMap<Named>::HandleEntryRef entityMirrors[createCount];

            std::vector<int> indexShuffle(createIndex + 1);
            std::iota(indexShuffle.begin(), indexShuffle.end(), 0);
            std::shuffle(indexShuffle.begin(), indexShuffle.end(),
                std::default_random_engine());

            auto GetHandleMap = [&](bool mirror) -> Common::HandleMap<Named>&
            {
                return mirror ? entitiesMirror : entities;
            };

            auto GetEntityIndex = [&](int index, bool mirror) -> int
            {
                return mirror ? indexShuffle[index] : index;
            };

            auto GetEntityEntry = [&](int index, bool mirror) ->
                Common::HandleMap<Named>::HandleEntryRef&
            {
                auto* entities = mirror ? entityMirrors : entityEntries;
                return entities[GetEntityIndex(index, mirror)];
            };

            auto GetRequestedHandle = [&](int index, bool mirror) -> Common::Handle<Named>
            {
                auto& original = GetEntityEntry(index, true);
                return mirror ? original.GetHandle() : Common::Handle<Named>();
            };

            for(int pass = 0; pass <= 1; ++pass)
            {
                bool mirrorPass = (pass == 1);
                SCOPED_TRACE(fmt::format("Mirror pass: {}", mirrorPass));

                for(int entityIndex = 0; entityIndex <= createIndex; ++entityIndex)
                {
                    SCOPED_TRACE(fmt::format("Entity index: {}", entityIndex));

                    auto& entityMap = GetHandleMap(mirrorPass);
                    auto& entityEntry = GetEntityEntry(entityIndex, mirrorPass);

                    auto createResult = entityMap.CreateHandle(
                        GetRequestedHandle(entityIndex, mirrorPass));

                    EXPECT_TRUE(createResult.IsSuccess());
                    entityEntry = createResult.Unwrap();

                    EXPECT_EQ(entityMap.GetValidHandleCount(), entityIndex + 1);

                    if(recreateIndex == 0)
                    {
                        EXPECT_EQ(entityMap.GetUnusedHandleCount(), cacheSize);
                    }
                    else
                    {
                        EXPECT_EQ(entityMap.GetUnusedHandleCount(),
                            std::min(cacheSize, cacheSize + createIndex - entityIndex));
                    }
                }

                for(int entityIndex = 0; entityIndex <= createIndex; ++entityIndex)
                {
                    SCOPED_TRACE(fmt::format("Entity index: {}", entityIndex));

                    auto& entityEntry = GetEntityEntry(entityIndex, mirrorPass);

                    EXPECT_NE(entityEntry.GetStorage(), nullptr);
                    EXPECT_TRUE(entityEntry.GetStorage()->name.empty());
                    entityEntry.GetStorage()->name = fmt::format("Entity{}",
                        GetEntityIndex(entityIndex, mirrorPass));

                    EXPECT_TRUE(entityEntry.GetHandle().IsValid());

                    if(recreateIndex == 0)
                    {
                        EXPECT_EQ(entityEntry.GetHandle().GetIdentifier(), entityIndex + 1);
                        EXPECT_EQ(entityEntry.GetHandle().GetVersion(), 0);
                    }
                }

                for(int entityIndex = 0; entityIndex <= createIndex; ++entityIndex)
                {
                    SCOPED_TRACE(fmt::format("Entity index: {}", entityIndex));

                    auto& entityMap = GetHandleMap(mirrorPass);
                    auto& entityEntry = GetEntityEntry(entityIndex, mirrorPass);

                    auto lookupResult = entityMap.LookupHandle(entityEntry.GetHandle());
                    EXPECT_TRUE(lookupResult.IsSuccess());

                    auto fetchedEntry = lookupResult.Unwrap();
                    EXPECT_EQ(fetchedEntry.GetHandle(), entityEntry.GetHandle());
                    EXPECT_EQ(fetchedEntry.GetStorage(), entityEntry.GetStorage());
                    EXPECT_EQ(fetchedEntry.GetStorage()->name,
                        fmt::format("Entity{}", GetEntityIndex(entityIndex, mirrorPass)));

                    if(mirrorPass)
                    {
                        EXPECT_EQ(entityEntry.GetHandle(),
                            GetRequestedHandle(entityIndex, mirrorPass));
                    }
                }
            }

            for(int pass = 0; pass <= 1; ++pass)
            {
                bool mirrorPass = (pass == 1);
                SCOPED_TRACE(fmt::format("Mirror pass: {}", mirrorPass));

                for(int entityIndex = 0; entityIndex <= createIndex; ++entityIndex)
                {
                    SCOPED_TRACE(fmt::format("Entity index: {}", entityIndex));

                    auto& entityMap = GetHandleMap(mirrorPass);
                    auto& entityEntry = GetEntityEntry(entityIndex, mirrorPass);

                    EXPECT_TRUE(entityMap.DestroyHandle(entityEntry.GetHandle()));
                    EXPECT_EQ(entityMap.GetValidHandleCount(), createIndex - entityIndex);
                    EXPECT_EQ(entityMap.GetUnusedHandleCount(), cacheSize + entityIndex + 1);
                }

                for(int entityIndex = 0; entityIndex <= createIndex; ++entityIndex)
                {
                    SCOPED_TRACE(fmt::format("Entity index: {}", entityIndex));

                    auto& entityMap = GetHandleMap(mirrorPass);
                    auto& entityEntry = GetEntityEntry(entityIndex, mirrorPass);

                    auto invalidatedEntry = entityMap.LookupHandle(entityEntry.GetHandle());
                    EXPECT_FALSE(invalidatedEntry.IsSuccess());

                    EXPECT_FALSE(entityMap.DestroyHandle(entityEntry.GetHandle()));
                }
            }

            EXPECT_EQ(entities.GetValidHandleCount(), 0);
            EXPECT_EQ(entities.GetUnusedHandleCount(), cacheSize + createIndex + 1);

            EXPECT_EQ(entitiesMirror.GetValidHandleCount(), 0);
            EXPECT_EQ(entitiesMirror.GetUnusedHandleCount(), cacheSize + createIndex + 1);
        }
    }
}

TEST(HandleMap, Iterators)
{
    std::vector<Common::Handle<Counter>> entityHandles;

    Common::HandleMap<Counter> entities;
    EXPECT_EQ(entities.GetValidHandleCount(), 0);
    EXPECT_EQ(entities.GetUnusedHandleCount(), 0);

    for(int i = 0; i < 10; ++i)
    {
        auto entityEntry = entities.CreateHandle().Unwrap();
        entityEntry.GetStorage()->counter = i;
        entityHandles.push_back(entityEntry.GetHandle());
    }

    EXPECT_TRUE(entities.DestroyHandle(entityHandles[0]));
    EXPECT_TRUE(entities.DestroyHandle(entityHandles[1]));
    EXPECT_TRUE(entities.DestroyHandle(entityHandles[3]));
    EXPECT_TRUE(entities.DestroyHandle(entityHandles[5]));
    EXPECT_TRUE(entities.DestroyHandle(entityHandles[9]));

    std::vector<Common::Handle<Counter>> valid;
    Common::HandleMap<Counter>::HandleEntryRef invalid;

    for(const auto& entityEntry : entities)
    {
        valid.push_back(entityEntry.GetHandle());
    }

    EXPECT_EQ(valid.size(), 5);
    EXPECT_EQ(entities.LookupHandle(valid[0])
        .UnwrapOr(invalid).GetHandle().GetIdentifier(), 3);
    EXPECT_EQ(entities.LookupHandle(valid[1])
        .UnwrapOr(invalid).GetHandle().GetIdentifier(), 5);
    EXPECT_EQ(entities.LookupHandle(valid[2])
        .UnwrapOr(invalid).GetHandle().GetIdentifier(), 7);
    EXPECT_EQ(entities.LookupHandle(valid[3])
        .UnwrapOr(invalid).GetHandle().GetIdentifier(), 8);
    EXPECT_EQ(entities.LookupHandle(valid[4])
        .UnwrapOr(invalid).GetHandle().GetIdentifier(), 9);

    std::vector<Common::Handle<Counter>> constValid;
    const Common::HandleMap<Counter>& constEntities = entities;

    for(const auto& entityEntry : constEntities)
    {
        constValid.push_back(entityEntry.GetHandle());
    }

    EXPECT_EQ(constValid.size(), 5);
    EXPECT_EQ(entities.LookupHandle(constValid[0])
        .UnwrapOr(invalid).GetHandle().GetIdentifier(), 3);
    EXPECT_EQ(entities.LookupHandle(constValid[1])
        .UnwrapOr(invalid).GetHandle().GetIdentifier(), 5);
    EXPECT_EQ(entities.LookupHandle(constValid[2])
        .UnwrapOr(invalid).GetHandle().GetIdentifier(), 7);
    EXPECT_EQ(entities.LookupHandle(constValid[3])
        .UnwrapOr(invalid).GetHandle().GetIdentifier(), 8);
    EXPECT_EQ(entities.LookupHandle(constValid[4])
        .UnwrapOr(invalid).GetHandle().GetIdentifier(), 9);
}
