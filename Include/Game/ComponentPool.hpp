/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <queue>
#include <vector>
#include <unordered_map>
#include "Game/EntityHandle.hpp"
#include "Game/Component.hpp"

/*
    Component Pool

    Manages a pool for a single type of component.
*/

namespace Game
{
    class ComponentSystem;

    class ComponentPoolInterface
    {
    protected:
        ComponentPoolInterface() = default;

    public:
        virtual ~ComponentPoolInterface() = default;
        virtual bool InitializeComponent(EntityHandle handle) = 0;
        virtual bool DestroyComponent(EntityHandle handle) = 0;
    };

    template<typename ComponentType>
    class ComponentPool final : public ComponentPoolInterface, private Common::NonCopyable
    {
    public:
        static_assert(std::is_base_of<Component, ComponentType>::value, "Not a component type.");

        struct ComponentFlags
        {
            enum
            {
                Unused = 0 << 0,        // Components is unused and waits in the free list.
                Exists = 1 << 0,        // Component exists and can be accessed.
                Initialized = 1 << 1,   // Component has been initialized and can be used.
            };

            using Type = uint8_t;
        };

        struct ComponentEntry
        {
            typename ComponentFlags::Type flags = ComponentFlags::Unused;
            EntityHandle entity;
            ComponentType component;
        };

        using ComponentIndex = std::size_t;
        using ComponentList = std::vector<ComponentEntry>;
        using ComponentFreeList = std::queue<ComponentIndex>;
        using ComponentLookup = std::unordered_map<EntityHandle, ComponentIndex>;

        class ComponentIterator
        {
        public:
            using BaseIterator = typename ComponentList::iterator;

        public:
            ComponentIterator(const BaseIterator& iterator, const BaseIterator& end);

            ComponentType& operator*();
            bool operator==(const ComponentIterator& other) const;
            bool operator!=(const ComponentIterator& other) const;
            ComponentIterator& operator++();

        private:
            void EnsureValid();

        private:
            BaseIterator m_iterator; // Iterator that we are wrapping around.
            BaseIterator m_end; // End of container that we are iterating over.
        };

        enum class CreateComponentErrors
        {
            AlreadyExists,
        };

        using CreateComponentResult = Common::Result<ComponentType*, CreateComponentErrors>;

        enum class LookupComponentErrors
        {
            Missing,
        };

        using LookupComponentResult = Common::Result<ComponentType*, LookupComponentErrors>;

    public:
        ComponentPool(ComponentSystem* componentSystem);
        ~ComponentPool();

        CreateComponentResult CreateComponent(EntityHandle entity);
        LookupComponentResult LookupComponent(EntityHandle entity);
        bool InitializeComponent(EntityHandle entity) override;
        bool DestroyComponent(EntityHandle entity) override;

        ComponentIterator Begin();
        ComponentIterator End();

    private:
        ComponentSystem* m_componentSystem = nullptr;
        ComponentList m_entries;
        ComponentLookup m_lookup;
        ComponentFreeList m_freeList;
    };

    template<typename ComponentType>
    void ComponentPool<ComponentType>::ComponentIterator::EnsureValid()
    {
        // Make sure that the current iterator is valid and if not, find the next iterator that is.
        while(m_iterator != m_end)
        {
            // Check if current iterator points at a valid component.
            if(m_iterator->flags & ComponentFlags::Initialized)
            {
                // Make sure component actually exists.
                ASSERT(m_iterator->flags & ComponentFlags::Exists,
                    "Component is not marked as existing despite being marked as initialized!");

                // Iterator is valid.
                break;
            }

            // Move iterator forward to the next element.
            ++m_iterator;
        }
    }

    template<typename ComponentType>
    ComponentPool<ComponentType>::ComponentIterator::ComponentIterator(const BaseIterator& iterator, const BaseIterator& end) :
        m_iterator(iterator), m_end(end)
    {
        this->EnsureValid();
    }

    template<typename ComponentType>
    ComponentType& ComponentPool<ComponentType>::ComponentIterator::operator*()
    {
        return m_iterator->component;
    }

    template<typename ComponentType>
    bool ComponentPool<ComponentType>::ComponentIterator::operator==(const ComponentIterator& other) const
    {
        return m_iterator == other.m_iterator;
    }

    template<typename ComponentType>
    bool ComponentPool<ComponentType>::ComponentIterator::operator!=(const ComponentIterator& other) const
    {
        return m_iterator != other.m_iterator;
    }

    template<typename ComponentType>
    typename ComponentPool<ComponentType>::ComponentIterator& ComponentPool<ComponentType>::ComponentIterator::operator++()
    {
        ASSERT(m_iterator != m_end, "Trying to increment component iterator past end!");

        ++m_iterator;
        EnsureValid();
        return *this;
    }

    template<typename ComponentType>
    ComponentPool<ComponentType>::ComponentPool(ComponentSystem* componentSystem) :
        m_componentSystem(componentSystem)
    {
        ASSERT(m_componentSystem != nullptr, "Component system cannot be null!");
    }

    template<typename ComponentType>
    ComponentPool<ComponentType>::~ComponentPool() = default;

    template<typename ComponentType>
    typename ComponentPool<ComponentType>::CreateComponentResult
        ComponentPool<ComponentType>::CreateComponent(EntityHandle entity)
    {
        // Make sure that there is no existing component with this entity handle.
        if(m_lookup.find(entity) != m_lookup.end())
        {
            return Common::Failure(
                ComponentPool<ComponentType>::CreateComponentErrors::AlreadyExists);
        }

        // Create new component entry if free list is empty.
        if(m_freeList.empty())
        {
            m_entries.emplace_back();
            m_freeList.emplace(m_entries.size() - 1);
        }

        // Retrieve an unused component index.
        ComponentIndex componentIndex = m_freeList.front();
        m_freeList.pop();

        // Add free component to entity lookup dictionary.
        auto result = m_lookup.emplace(entity, componentIndex);
        ASSERT(result.second, "Failed to add a component to look up dictionary!");

        // Retrieve component entry and mark is as existing.
        ComponentEntry& componentEntry = m_entries[componentIndex];
        ASSERT(componentEntry.flags == ComponentFlags::Unused);
        componentEntry.flags = ComponentFlags::Exists;
        return Common::Success(&componentEntry.component);
    }

    template<typename ComponentType>
    typename ComponentPool<ComponentType>::LookupComponentResult
        ComponentPool<ComponentType>::LookupComponent(EntityHandle handle)
    {
        // Find component index using entity handle.
        auto it = m_lookup.find(handle);
        if(it == m_lookup.end())
        {
            return Common::Failure(
                ComponentPool<ComponentType>::LookupComponentErrors::Missing);
        }

        // Retrieve component entry using the index.
        ComponentIndex componentIndex = it->second;
        ComponentEntry& componentEntry = m_entries[componentIndex];
        ASSERT(componentEntry.flags & ComponentFlags::Exists);
        return Common::Success(&componentEntry.component);
    }

    template<typename ComponentType>
    bool ComponentPool<ComponentType>::InitializeComponent(EntityHandle entity)
    {
        // Find component index using entity handle.
        // If component does not exist, consider initialization a non-failure scenario.
        auto it = m_lookup.find(entity);
        if(it == m_lookup.end())
            return true;

        // Retrieve component entry using the index.
        ComponentIndex componentIndex = it->second;
        ComponentEntry& componentEntry = m_entries[componentIndex];
        ASSERT(componentEntry.flags & ComponentFlags::Exists);
        ASSERT(!(componentEntry.flags & ComponentFlags::Initialized));

        // Initialize component and return result.
        Component& componentInterface = componentEntry.component;
        ASSERT(m_componentSystem != nullptr, "Component system cannot be null!");
        if(!componentInterface.OnInitialize(m_componentSystem, entity))
            return false;

        // Mark component as initialized.
        componentEntry.flags |= ComponentFlags::Initialized;
        return true;
    }

    template<typename ComponentType>
    bool ComponentPool<ComponentType>::DestroyComponent(EntityHandle entity)
    {
        // Find component index using entity handle.
        auto it = m_lookup.find(entity);
        if(it == m_lookup.end())
            return false;

        // Retrieve component entry using the index.
        ComponentIndex componentIndex = it->second;
        ComponentEntry& componentEntry = m_entries[componentIndex];

        // Mark component as unused.
        ASSERT(componentEntry.flags & ComponentFlags::Exists);
        componentEntry.flags = ComponentFlags::Unused;

        // Recreate component storage in place.
        ComponentType* component = &componentEntry.component;
        component->~ComponentType();
        new (component) ComponentType();

        // Add unused component index to free list and remove it from lookup dictionary.
        m_freeList.emplace(componentIndex);
        m_lookup.erase(it);
        return true;
    }

    template<typename ComponentType>
    typename ComponentPool<ComponentType>::ComponentIterator ComponentPool<ComponentType>::Begin()
    {
        return ComponentIterator(m_entries.begin(), m_entries.end());
    }

    template<typename ComponentType>
    typename ComponentPool<ComponentType>::ComponentIterator ComponentPool<ComponentType>::End()
    {
        return ComponentIterator(m_entries.end(), m_entries.end());
    }

    template<typename ComponentType>
    typename ComponentPool<ComponentType>::ComponentIterator begin(ComponentPool<ComponentType>& pool)
    {
        return pool.Begin();
    }

    template<typename ComponentType>
    typename ComponentPool<ComponentType>::ComponentIterator end(ComponentPool<ComponentType>& pool)
    {
        return pool.End();
    }
}
