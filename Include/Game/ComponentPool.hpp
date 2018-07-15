/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include <unordered_map>
#include "Memory/SlotArray.hpp"
#include "Game/EntitySystem.hpp"
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

        // Type declarations.
        using ComponentArray = Common::SlotArray<ComponentType>;
        using ComponentIterator = typename ComponentArray::Iterator;
        using ComponentHandle = typename ComponentArray::HandleType;
        using ComponentLookup = std::unordered_map<EntityHandle, ComponentHandle>;

    public:
        ComponentPool();
        ~ComponentPool();

        // Creates a component.
        // Returns nullptr if component could not be created.
        ComponentType* CreateComponent(EntityHandle entityHandle);

        // Lookups a component.
        // Returns nullptr if component could not be found.
        ComponentType* LookupComponent(EntityHandle entityHandle);

        // Destroys a component.
        // Returns true if component was found and destroyed.
        bool DestroyComponent(EntityHandle entityHandle) override;

        // Get the begin iterator.
        ComponentIterator Begin();
        
        // Gets the end iterator.
        ComponentIterator End();

    private:
        // List of components.
        ComponentArray m_components;
        ComponentLookup m_lookup;
    };

    // Template definitions.
    template<typename ComponentType>
    ComponentPool<ComponentType>::ComponentPool()
    {
    }

    template<typename ComponentType>
    ComponentPool<ComponentType>::~ComponentPool()
    {
    }

    template<typename ComponentType>
    ComponentType* ComponentPool<ComponentType>::CreateComponent(EntityHandle entityHandle)
    {
        // Make sure that there is no component with this entity handle.
        if(m_lookup.find(entityHandle) != m_lookup.end())
            return nullptr;

        // Create a new component.
        ComponentHandle componentHandle = m_components.Create();

        // Add newly created component to the lookup dictionary.
        auto result = m_lookup.emplace(entityHandle, componentHandle);
        ASSERT(result.second, "Failed to add a component to the dictionary!");

        // Return newly created component
        return m_components.Lookup(componentHandle);
    }

    template<typename ComponentType>
    ComponentType* ComponentPool<ComponentType>::LookupComponent(EntityHandle entityHandle)
    {
        // Find the component.
        auto it = m_lookup.find(entityHandle);
        if(it == m_lookup.end())
            return nullptr;

        // Return a pointer to the component.
        return m_components.Lookup(it->second);
    }

    template<typename ComponentType>
    bool ComponentPool<ComponentType>::DestroyComponent(EntityHandle handle)
    {
        // Find the component.
        auto it = m_lookup.find(handle);
        if(it == m_lookup.end())
            return false;

        // Remove component entry from the array.
        bool result = m_components.Remove(it->second);
        ASSERT(result, "Failed to remove a known component from the array!");

        // Remove component entry from the lookup.
        m_lookup.erase(it);

        return true;
    }

    template<typename ComponentType>
    typename ComponentPool<ComponentType>::ComponentIterator ComponentPool<ComponentType>::Begin()
    {
        return m_components.Begin();
    }

    template<typename ComponentType>
    typename ComponentPool<ComponentType>::ComponentIterator ComponentPool<ComponentType>::End()
    {
        return m_components.End();
    }
}
