/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include <queue>
#include <unordered_map>
#include "Game/EntityHandle.hpp"
#include "Game/Component.hpp"

/*
    Component Pool
    
    Manages a pool for a single type of a component.
    See ComponentSystem for more context.
*/

namespace Game
{
    // Component pool interface class.
    class ComponentPoolInterface
    {
    protected:
        ComponentPoolInterface()
        {
        }

    public:
        virtual ~ComponentPoolInterface()
        {
        }

        virtual bool DestroyComponent(EntityHandle handle) = 0;
    };

    // Component pool class.
    template<typename ComponentType>
    class ComponentPool : public ComponentPoolInterface
    {
    public:
        // Check template type.
        static_assert(std::is_base_of<Component, ComponentType>::value, "Not a component type.");

        // Component entry structure.
        struct ComponentEntry
        {
            ComponentEntry();

            bool exists;
            EntityHandle entity;
            ComponentType component;
        };

        // Type declarations.
        using ComponentIndex = std::size_t;
        using ComponentList = std::vector<ComponentEntry>;
        using ComponentIterator = typename ComponentList::iterator;
        using ComponentFreeList = std::queue<ComponentIndex>;
        using ComponentLookup = std::unordered_map<EntityHandle, ComponentIndex>;

    public:
        ComponentPool();
        ~ComponentPool();

        // Disallow copying.
        ComponentPool(const ComponentPool& other) = delete;
        ComponentPool& operator=(const ComponentPool& other) = delete;

        // Move constructor and assignment.
        ComponentPool(ComponentPool&& other);
        ComponentPool& operator=(ComponentPool&& other);

        // Creates a component.
        // Returns nullptr if component could not be created.
        ComponentType* CreateComponent(EntityHandle entity);

        // Lookups a component.
        // Returns nullptr if component could not be found.
        ComponentType* LookupComponent(EntityHandle entity);

        // Destroys a component.
        // Returns true if component was found and destroyed.
        bool DestroyComponent(EntityHandle entity) override;

        // Get the begin iterator for component entries.
        ComponentIterator Begin();
        
        // Gets the end iterator for component entries.
        ComponentIterator End();

    private:
        // List of components.
        ComponentList m_entries;

        // Lookup dictionary.
        ComponentLookup m_lookup;

        // Free list queue.
        ComponentFreeList m_freeList;
    };

    // Template definitions.
    template<typename ComponentType>
    ComponentPool<ComponentType>::ComponentEntry::ComponentEntry() :
        exists(false),
        entity(),
        component()
    {
    }

    template<typename ComponentType>
    ComponentPool<ComponentType>::ComponentPool()
    {
    }

    template<typename ComponentType>
    ComponentPool<ComponentType>::~ComponentPool()
    {
    }

    template<typename ComponentType>
    ComponentPool<ComponentType>::ComponentPool(ComponentPool&& other) :
        CompoentPool<ComponentType>()
    {
        // Call the move assignment.
        *this = std::move(other);
    }

    template<typename ComponentType>
    ComponentPool<ComponentType>& ComponentPool<ComponentType>::operator=(ComponentPool&& other)
    {
        // Swap class members.
        std::swap(m_entries, other.m_entries);
        std::swap(m_lookup, other.m_lookup);
        std::swap(m_freeList, other.m_freeList);

        return *this;/
    }

    template<typename ComponentType>
    ComponentType* ComponentPool<ComponentType>::CreateComponent(EntityHandle entity)
    {
        // Make sure that there is no component with this entity handle.
        if(m_lookup.find(entity) != m_lookup.end())
            return nullptr;

        // Create a new component entry if the free list is empty.
        if(m_freeList.empty())
        {
            // Create a new component entry.
            m_entries.emplace_back();

            // Add a new entry to the free list queue.
            m_freeList.emplace(m_entries.size() - 1);
        }

        // Retrieve an unused component index.
        ComponentIndex componentIndex = m_freeList.front();
        m_freeList.pop();

        // Add newly created component to the lookup dictionary.
        auto result = m_lookup.emplace(entity, componentIndex);
        ASSERT(result.second, "Failed to add a component to the dictionary!");

        // Retrieve a component entry.
        ComponentEntry& componentEntry = m_entries[componentIndex];

        // Mark component as created.
        ASSERT(!componentEntry.exists, "Creating component that is alrady marked as existing!");
        componentEntry.exists = true;

        // Return newly created component
        return &componentEntry.component;
    }

    template<typename ComponentType>
    ComponentType* ComponentPool<ComponentType>::LookupComponent(EntityHandle handle)
    {
        // Find the component index.
        auto it = m_lookup.find(handle);
        if(it == m_lookup.end())
            return nullptr;

        ComponentIndex componentIndex = it->second;

        // Retrieve the component entry.
        ComponentEntry& componentEntry = m_entries[componentIndex];

        // Return a pointer to the component.
        return &componentEntry.component;
    }

    template<typename ComponentType>
    bool ComponentPool<ComponentType>::DestroyComponent(EntityHandle handle)
    {
        // Find the component index.
        auto it = m_lookup.find(handle);
        if(it == m_lookup.end())
            return false;

        ComponentIndex componentIndex = it->second;

        // Retrieve the component entry.
        ComponentEntry& componentEntry = m_entries[componentIndex];

        // Mark component as not created.
        ASSERT(componentEntry.exists, "Destroying component that is not marked as existing!");
        componentEntry.exists = false;

        // Recreate component instance to trigger a destructor and create a new element.
        ComponentType* component = &componentEntry.component;

        component->~ComponentType();
        new (component) ComponentType();

        // Add an unused component index to the free list.
        m_freeList.emplace(componentIndex);

        // Remove component entry from the dictionary.
        m_lookup.erase(it);

        return true;
    }

    template<typename ComponentType>
    typename ComponentPool<ComponentType>::ComponentIterator ComponentPool<ComponentType>::Begin()
    {
        return m_entries.begin();
    }

    template<typename ComponentType>
    typename ComponentPool<ComponentType>::ComponentIterator ComponentPool<ComponentType>::End()
    {
        return m_entries.end();
    }
}
