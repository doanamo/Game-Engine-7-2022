/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include <random>
#include <fmt/core.h>
#include <doctest/doctest.h>
#include <Common/HandleMap.hpp>

TEST_CASE("Handle")
{
    struct Empty
    {
    };

    Common::Handle<Empty> handleA;
    CHECK_FALSE(handleA.IsValid());
    CHECK_EQ(handleA.GetIdentifier(), 0);
    CHECK_EQ(handleA.GetVersion(), 0);

    Common::Handle<Empty> handleB;
    CHECK_EQ(handleA, handleB);
    CHECK_EQ(handleB, handleA);
    CHECK_FALSE(handleA != handleB);
    CHECK_FALSE(handleB != handleA);
    CHECK_FALSE(handleA < handleB);
    CHECK_FALSE(handleB < handleA);
}

TEST_CASE("Handle Map")
{
    struct Entity
    {
        std::string name;
    };

    for(int cacheSize : { 0, 1, 3, 8 })
    SUBCASE(fmt::format("With cache size {}", cacheSize).c_str())
    {
        Common::HandleMap<Entity> entities(cacheSize);
        Common::HandleMap<Entity> entitiesMirror(cacheSize);

        SUBCASE("Empty")
        {
            CHECK_EQ(entities.GetValidHandleCount(), 0);
            CHECK_EQ(entities.GetUnusedHandleCount(), 0);
        }

        SUBCASE("Invalid handle")
        {
            CHECK_FALSE(entities.LookupHandle(Common::Handle<Entity>()).IsSuccess());
            CHECK_FALSE(entities.DestroyHandle(Common::Handle<Entity>()));
        }

        SUBCASE("Handle creation")
        {
            const int createCount = 4;
            const int recreateCount = cacheSize + 1;

            for(int createIndex = 0; createIndex < createCount; ++createIndex)
            SUBCASE(fmt::format("Create {} handle(s) and mirror them", createIndex + 1).c_str())
            {
                for(int recreateIndex = 0; recreateIndex <= recreateCount; ++recreateIndex)
                SUBCASE(recreateIndex ? fmt::format("Recreate handle(s) (iteration #{})",
                    recreateIndex).c_str() : "")
                {
                    Common::HandleMap<Entity>::HandleEntryRef entityEntries[createCount];
                    Common::HandleMap<Entity>::HandleEntryRef entityMirrors[createCount];

                    std::vector<int> indexShuffle(createIndex + 1);
                    std::iota(indexShuffle.begin(), indexShuffle.end(), 0);
                    std::shuffle(indexShuffle.begin(), indexShuffle.end(),
                        std::default_random_engine());

                    auto GetHandleMap = [&](bool mirror) -> Common::HandleMap<Entity>&
                    {
                        return mirror ? entitiesMirror : entities;
                    };

                    auto GetEntityIndex = [&](int index, bool mirror) -> int
                    {
                        return mirror ? indexShuffle[index] : index;
                    };

                    auto GetEntityEntry = [&](int index, bool mirror) ->
                        Common::HandleMap<Entity>::HandleEntryRef&
                    {
                        auto* entities = mirror ? entityMirrors : entityEntries;
                        return entities[GetEntityIndex(index, mirror)];
                    };

                    auto GetRequestedHandle = [&](int index, bool mirror) -> Common::Handle<Entity>
                    {
                        auto& original = GetEntityEntry(index, true);
                        return mirror ? original.GetHandle() : Common::Handle<Entity>();
                    };

                    for(int pass = 0; pass <= 1; ++pass)
                    {
                        bool mirrorPass = (pass == 1);
                        CAPTURE(mirrorPass);

                        for(int entityIndex = 0; entityIndex <= createIndex; ++entityIndex)
                        {
                            CAPTURE(entityIndex);

                            auto& entityMap = GetHandleMap(mirrorPass);
                            auto& entityEntry = GetEntityEntry(entityIndex, mirrorPass);

                            auto createResult = entityMap.CreateHandle(
                                GetRequestedHandle(entityIndex, mirrorPass));

                            REQUIRE(createResult.IsSuccess());
                            entityEntry = createResult.Unwrap();

                            CHECK_EQ(entityMap.GetValidHandleCount(), entityIndex + 1);

                            if(recreateIndex == 0)
                            {
                                CHECK_EQ(entityMap.GetUnusedHandleCount(), cacheSize);
                            }
                            else
                            {
                                CHECK_EQ(entityMap.GetUnusedHandleCount(),
                                    std::min(cacheSize, cacheSize + createIndex - entityIndex));
                            }
                        }

                        for(int entityIndex = 0; entityIndex <= createIndex; ++entityIndex)
                        {
                            CAPTURE(entityIndex);

                            auto& entityEntry = GetEntityEntry(entityIndex, mirrorPass);

                            CHECK_NE(entityEntry.GetStorage(), nullptr);
                            CHECK(entityEntry.GetStorage()->name.empty());
                            entityEntry.GetStorage()->name = fmt::format("Entity{}",
                                GetEntityIndex(entityIndex, mirrorPass));

                            CHECK(entityEntry.GetHandle().IsValid());

                            if(recreateIndex == 0)
                            {
                                CHECK_EQ(entityEntry.GetHandle().GetIdentifier(), entityIndex + 1);
                                CHECK_EQ(entityEntry.GetHandle().GetVersion(), 0);
                            }
                        }

                        for(int entityIndex = 0; entityIndex <= createIndex; ++entityIndex)
                        {
                            CAPTURE(entityIndex);

                            auto& entityMap = GetHandleMap(mirrorPass);
                            auto& entityEntry = GetEntityEntry(entityIndex, mirrorPass);

                            auto lookupResult = entityMap.LookupHandle(entityEntry.GetHandle());
                            REQUIRE(lookupResult.IsSuccess());

                            auto fetchedEntry = lookupResult.Unwrap();
                            CHECK_EQ(fetchedEntry.GetHandle(), entityEntry.GetHandle());
                            CHECK_EQ(fetchedEntry.GetStorage(), entityEntry.GetStorage());
                            CHECK_EQ(fetchedEntry.GetStorage()->name,
                                fmt::format("Entity{}", GetEntityIndex(entityIndex, mirrorPass)));

                            if(mirrorPass)
                            {
                                CHECK_EQ(entityEntry.GetHandle(),
                                    GetRequestedHandle(entityIndex, mirrorPass));
                            }
                        }
                    }

                    for(int pass = 0; pass <= 1; ++pass)
                    {
                        bool mirrorPass = (pass == 1);
                        CAPTURE(mirrorPass);

                        for(int entityIndex = 0; entityIndex <= createIndex; ++entityIndex)
                        {
                            CAPTURE(entityIndex);

                            auto& entityMap = GetHandleMap(mirrorPass);
                            auto& entityEntry = GetEntityEntry(entityIndex, mirrorPass);

                            CHECK(entityMap.DestroyHandle(entityEntry.GetHandle()));

                            CHECK_EQ(entityMap.GetValidHandleCount(), createIndex - entityIndex);
                            CHECK_EQ(entityMap.GetUnusedHandleCount(), cacheSize + entityIndex + 1);
                        }

                        for(int entityIndex = 0; entityIndex <= createIndex; ++entityIndex)
                        {
                            auto& entityMap = GetHandleMap(mirrorPass);
                            auto& entityEntry = GetEntityEntry(entityIndex, mirrorPass);

                            auto invalidatedEntry = entityMap.LookupHandle(entityEntry.GetHandle());
                            CHECK_FALSE(invalidatedEntry.IsSuccess());

                            CHECK_FALSE(entityMap.DestroyHandle(entityEntry.GetHandle()));
                        }
                    }
                }

                CHECK_EQ(entities.GetValidHandleCount(), 0);
                CHECK_EQ(entities.GetUnusedHandleCount(), cacheSize + createIndex + 1);

                CHECK_EQ(entitiesMirror.GetValidHandleCount(), 0);
                CHECK_EQ(entitiesMirror.GetUnusedHandleCount(), cacheSize + createIndex + 1);
            }
        }
    }
}

TEST_CASE("Handle Map Iterators")
{
    struct Entity
    {
        int counter = 0;
    };

    std::vector<Common::Handle<Entity>> entityHandles;

    Common::HandleMap<Entity> entities;
    CHECK_EQ(entities.GetValidHandleCount(), 0);
    CHECK_EQ(entities.GetUnusedHandleCount(), 0);

    for(int i = 0; i < 10; ++i)
    {
        auto entityEntry = entities.CreateHandle().Unwrap();
        entityEntry.GetStorage()->counter = i;
        entityHandles.push_back(entityEntry.GetHandle());
    }

    CHECK(entities.DestroyHandle(entityHandles[0]));
    CHECK(entities.DestroyHandle(entityHandles[1]));
    CHECK(entities.DestroyHandle(entityHandles[3]));
    CHECK(entities.DestroyHandle(entityHandles[5]));
    CHECK(entities.DestroyHandle(entityHandles[9]));

    std::vector<Common::Handle<Entity>> valid;
    Common::HandleMap<Entity>::HandleEntryRef invalid;

    for(const auto& entityEntry : entities)
    {
        valid.push_back(entityEntry.GetHandle());
    }

    CHECK_EQ(valid.size(), 5);
    CHECK_EQ(entities.LookupHandle(valid[0]).UnwrapOr(invalid).GetHandle().GetIdentifier(), 3);
    CHECK_EQ(entities.LookupHandle(valid[1]).UnwrapOr(invalid).GetHandle().GetIdentifier(), 5);
    CHECK_EQ(entities.LookupHandle(valid[2]).UnwrapOr(invalid).GetHandle().GetIdentifier(), 7);
    CHECK_EQ(entities.LookupHandle(valid[3]).UnwrapOr(invalid).GetHandle().GetIdentifier(), 8);
    CHECK_EQ(entities.LookupHandle(valid[4]).UnwrapOr(invalid).GetHandle().GetIdentifier(), 9);

    std::vector<Common::Handle<Entity>> constValid;
    const Common::HandleMap<Entity>& constEntities = entities;

    for(const auto& entityEntry : constEntities)
    {
        constValid.push_back(entityEntry.GetHandle());
    }

    CHECK_EQ(constValid.size(), 5);
    CHECK_EQ(entities.LookupHandle(constValid[0]).UnwrapOr(invalid).GetHandle().GetIdentifier(), 3);
    CHECK_EQ(entities.LookupHandle(constValid[1]).UnwrapOr(invalid).GetHandle().GetIdentifier(), 5);
    CHECK_EQ(entities.LookupHandle(constValid[2]).UnwrapOr(invalid).GetHandle().GetIdentifier(), 7);
    CHECK_EQ(entities.LookupHandle(constValid[3]).UnwrapOr(invalid).GetHandle().GetIdentifier(), 8);
    CHECK_EQ(entities.LookupHandle(constValid[4]).UnwrapOr(invalid).GetHandle().GetIdentifier(), 9);
}
