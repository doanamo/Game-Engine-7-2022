/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <memory>

/*
    Instance Counter
*/

namespace Test
{
    template<typename Type = int>
    class InstanceCounter
    {
    public:
        struct Stats
        {
            int instances = 0;
            int constructions = 0;
            int destructions = 0;
            int copies = 0;
            int moves = 0;
        };

    public:
        InstanceCounter() :
            m_stats(std::make_shared<Stats>())
        {
            m_stats->instances += 1;
            m_stats->constructions += 1;
        }

        InstanceCounter(const InstanceCounter& other) :
            m_instance(other.m_instance),
            m_stats(other.m_stats)
        {
            m_stats->instances += 1;
            m_stats->constructions += 1;
            m_stats->copies += 1;
        }

        InstanceCounter(InstanceCounter&& other) :
            m_instance(std::move(other.m_instance)),
            m_stats(other.m_stats)
        {
            m_stats->instances += 1;
            m_stats->constructions += 1;
            m_stats->moves += 1;
        }

        ~InstanceCounter()
        {
            m_stats->instances -= 1;
            m_stats->destructions += 1;
        }

        InstanceCounter& operator=(const InstanceCounter& other)
        {
            REQUIRE(&other != this);

            m_instance = other.m_instance;
            m_stats = other.m_stats;
            m_stats->copies += 1;

            return *this;
        }

        InstanceCounter& operator=(InstanceCounter&& other)
        {
            REQUIRE(&other != this);

            std::swap(m_instance, other.m_instance);
            std::swap(m_stats, other.m_stats);
            m_stats->moves += 1;

            return *this;
        }

        Type& GetInstance()
        {
            REQUIRE(m_instance.get() != nullptr);
            return *m_instance;
        }

        Stats& GetStats() const
        {
            REQUIRE(m_stats.get() != nullptr);
            return *m_stats;
        }

    private:
        Type m_instance;
        std::shared_ptr<Stats> m_stats;
    };
}
