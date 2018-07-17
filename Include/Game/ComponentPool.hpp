/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

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
        typedef std::size_t ComponentIndex;

        struct ComponentEntry
        {
            ComponentEntry();

            bool created;
            ComponentType component;
            ComponentIndex nextFree;
        };

        // Type declarations.
        typedef std::unordered_map<EntityHandle, ComponentIndex> ComponentDictionary;
        typedef std::vector<ComponentEntry> ComponentList;
        typedef typename ComponentList::iterator ComponentIterator;

        // Constant definitions.
        static const ComponentIndex InvalidIndex = -1;

    public:
        ComponentPool();
        ~ComponentPool();

        // Creates a component.
        // Returns nullptr if component could not be created.
        ComponentType* CreateComponent(EntityHandle handle);

        // Lookups a component.
        // Returns nullptr if component could not be found.
        ComponentType* LookupComponent(EntityHandle handle);

        // Destroys a component.
        // Returns true if component was found and destroyed.
        bool DestroyComponent(EntityHandle handle) override;

        // Get the begin iterator.
        ComponentIterator Begin();
        
        // Gets the end iterator.
        ComponentIterator End();

    private:
        // List of components.
        ComponentDictionary m_dictionary;
        ComponentList m_components;

        // List of free component entries.
        ComponentIndex m_freeListDequeue;
        ComponentIndex m_freeListEnqueue;
    };

    // Template definitions.
    template<typename ComponentType>
    ComponentPool<ComponentType>::ComponentEntry::ComponentEntry() :
        created(false),
        component(),
        nextFree(InvalidIndex)
    {
    }

    template<typename ComponentType>
    ComponentPool<ComponentType>::ComponentPool() :
        m_freeListEnqueue(InvalidIndex),
        m_freeListDequeue(InvalidIndex)
    {
    }

    template<typename ComponentType>
    ComponentPool<ComponentType>::~ComponentPool()
    {
    }

    template<typename ComponentType>
    ComponentType* ComponentPool<ComponentType>::CreateComponent(EntityHandle handle)
    {
        // Make sure that there is no component with this entity handle.
        if(m_dictionary.find(handle) != m_dictionary.end())
            return nullptr;

        // Create a new component entry if the free list is empty.
        if(m_freeListDequeue == InvalidIndex)
        {
            // Create a new component entry.
            m_components.emplace_back();

            // Make a new entry the only element in the free list.
            m_freeListDequeue = m_freeListEnqueue = m_components.size() - 1;
        }

        // Retrieve a free component entry and remove it from the free list.
        ComponentIndex componentIndex = m_freeListDequeue;
        ComponentEntry& componentEntry = m_components[componentIndex];

        if(m_freeListDequeue == m_freeListEnqueue)
        {
            // Remove the only entry from the free list.
            m_freeListDequeue = InvalidIndex;
            m_freeListEnqueue = InvalidIndex;
        }
        else
        {
            // Remove from the beginning of the free list.
            ASSERT(componentEntry.nextFree != InvalidIndex, "Component entry is missing next free index!");

            m_freeListDequeue = componentEntry.nextFree;
            componentEntry.nextFree = InvalidIndex;
        }

        // Add newly created component to the lookup dictionary.
        auto result = m_dictionary.emplace(std::piecewise_construct, std::forward_as_tuple(handle), std::forward_as_tuple(componentIndex));
        ASSERT(result.second, "Failed to add a component to the dictionary!");

        // Mark component as created.
        ASSERT(!componentEntry.created, "Component is already marked as created!");
        componentEntry.created = true;

        // Return newly created component
        return &componentEntry.component;
    }

    template<typename ComponentType>
    ComponentType* ComponentPool<ComponentType>::LookupComponent(EntityHandle handle)
    {
        // Find the component.
        auto it = m_dictionary.find(handle);
        if(it == m_dictionary.end())
            return nullptr;

        // Retrieve the component entry.
        ComponentEntry& componentEntry = m_components[it->second];

        // Return a pointer to the component.
        return &componentEntry.component;
    }

    template<typename ComponentType>
    bool ComponentPool<ComponentType>::DestroyComponent(EntityHandle handle)
    {
        // Find the component.
        auto it = m_dictionary.find(handle);
        if(it == m_dictionary.end())
            return false;

        // Retrieve the component entry.
        ComponentEntry& componentEntry = m_components[it->second];

        // Mark component as not created.
        ASSERT(componentEntry.created, "Destroying component that is not marked as created!");
        componentEntry.created = false;

        // Reset component instance to its default state.
        componentEntry.component = ComponentType();

        // Add component entry to the free list.
        if(m_freeListDequeue == m_freeListEnqueue)
        {
            // Make component entry index as the only element in the free list. 
            m_freeListDequeue = it->second;
            m_freeListEnqueue = it->second;
        }
        else
        {
            // Add component entry index at the end of the free list.
            ComponentEntry& lastFreeEntry = m_components[m_freeListEnqueue];

            lastFreeEntry.nextFree = it->second;
            m_freeListEnqueue = it->second;
        }

        // Remove component entry from the dictionary.
        m_dictionary.erase(it);

        return true;
    }

    template<typename ComponentType>
    typename ComponentPool<ComponentType>::ComponentIterator ComponentPool<ComponentType>::Begin()
    {
        return m_components.begin();
    }

    template<typename ComponentType>
    typename ComponentPool<ComponentType>::ComponentIterator ComponentPool<ComponentType>::End()
    {
        return m_components.end();
    }
}
