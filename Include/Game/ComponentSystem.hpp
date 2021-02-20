/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>
#include <Event/Receiver.hpp>
#include "Game/EntitySystem.hpp"
#include "Game/ComponentPool.hpp"

/*
    Component System

    Manages component types and their instances.
*/

namespace Game
{
    class EntitySystem;

    class ComponentSystem final : private Common::NonCopyable
    {
    public:
        enum class CreateErrors
        {
            InvalidArgument,
            FailedEventSubscription,
        };

        using CreateResult = Common::Result<std::unique_ptr<ComponentSystem>, CreateErrors>;
        static CreateResult Create(EntitySystem* entitySystem);

        using ComponentPoolPtr = std::unique_ptr<ComponentPoolInterface>;
        using ComponentPoolList = std::unordered_map<std::type_index, ComponentPoolPtr>;
        using ComponentPoolPair = ComponentPoolList::value_type;

    public:
        ~ComponentSystem();

        template<typename ComponentType>
        ComponentType* Create(EntityHandle handle);

        template<typename ComponentType>
        ComponentType* Lookup(EntityHandle handle);

        template<typename ComponentType>
        ComponentPool<ComponentType>& GetPool();

        template<typename ComponentType>
        typename ComponentPool<ComponentType>::ComponentIterator Begin();

        template<typename ComponentType>
        typename ComponentPool<ComponentType>::ComponentIterator End();

        EntitySystem* GetEntitySystem() const;

    private:
        ComponentSystem();

        template<typename ComponentType>
        bool Destroy(EntityHandle handle);

        template<typename ComponentType>
        ComponentPool<ComponentType>* CreatePool();

        Event::Receiver<bool(EntityHandle)> m_entityCreate;
        Event::Receiver<void(EntityHandle)> m_entityDestroy;

        bool OnEntityCreate(EntityHandle handle);
        void OnEntityDestroy(EntityHandle handle);

    private:
        EntitySystem* m_entitySystem = nullptr;
        ComponentPoolList m_pools;
    };

    template<typename ComponentType>
    ComponentType* ComponentSystem::Create(EntityHandle handle)
    {
        // Validate component type.
        static_assert(std::is_base_of<Component, ComponentType>::value, "Not a component type.");

        // Retrieve entity entry to determine if handle is valid.
        const EntitySystem::EntityEntry* entityEntry = m_entitySystem->GetEntityEntry(handle);
        if(!entityEntry)
            return nullptr;

        // Get component pool.
        ComponentPool<ComponentType>& pool = this->GetPool<ComponentType>();

        // Create new component.
        ComponentType* component = pool.CreateComponent(handle);

        if(component != nullptr)
        {
            // Check if entity has already been created and has its components initialized.
            // If entity has already been created, initialize the component right away.
            if(entityEntry->flags & EntitySystem::EntityFlags::Created)
            {
                // Initialize component.
                if(!pool.InitializeComponent(handle))
                {
                    // Destroy component if initialization fails.
                    bool destroyResult = pool.DestroyComponent(handle);
                    ASSERT(destroyResult, "Could not destroy component!");
                    return nullptr;
                }
            }
        }

        // Return the created component.
        return component;
    }

    template<typename ComponentType>
    ComponentType* ComponentSystem::Lookup(EntityHandle handle)
    {
        // Validate component type.
        static_assert(std::is_base_of<Component, ComponentType>::value, "Not a component type.");

        // Get component pool.
        ComponentPool<ComponentType>& pool = this->GetPool<ComponentType>();

        // Lookup and return the component.
        return pool.LookupComponent(handle);
    }

    template<typename ComponentType>
    bool ComponentSystem::Destroy(EntityHandle handle)
    {
        // Validate component type.
        static_assert(std::is_base_of<Component, ComponentType>::value, "Not a component type.");

        // Get component pool.
        ComponentPool<ComponentType>* pool = this->GetPool<ComponentType>();
        ASSERT(pool != nullptr, "Retrieved a null component pool!");

        // Destroy the component.
        return pool->DestroyComponent(handle);
    }

    template<typename ComponentType>
    ComponentPool<ComponentType>& ComponentSystem::GetPool()
    {
        // Validate component type.
        static_assert(std::is_base_of<Component, ComponentType>::value, "Not a component type.");

        // Find pool by component type.
        auto it = m_pools.find(typeid(ComponentType));
        if(it == m_pools.end())
        {
            // Create and return a new component pool.
            auto* pool = this->CreatePool<ComponentType>();
            ASSERT(pool, "Failed to create component pool!");
            return *pool;
        }

        // Cast and return the pointer that we already know is a component pool.
        auto* pool = reinterpret_cast<ComponentPool<ComponentType>*>(it->second.get());
        ASSERT(pool, "Component systems contains null component pool!");
        return *pool;
    }

    template<typename ComponentType>
    ComponentPool<ComponentType>* ComponentSystem::CreatePool()
    {
        // Validate component type.
        static_assert(std::is_base_of<Component, ComponentType>::value, "Not a component type.");

        // Create and add pool to the collection.
        auto pool = std::make_unique<ComponentPool<ComponentType>>(this);
        auto result = m_pools.emplace(std::piecewise_construct,
            std::forward_as_tuple(typeid(ComponentType)),
            std::forward_as_tuple(std::move(pool))
        );

        ASSERT(result.second == true, "Failed to insert new component pool type!");

        // Return created pool.
        return reinterpret_cast<ComponentPool<ComponentType>*>(result.first->second.get());
    }

    template<typename ComponentType>
    typename ComponentPool<ComponentType>::ComponentIterator ComponentSystem::Begin()
    {
        // Validate component type.
        static_assert(std::is_base_of<Component, ComponentType>::value, "Not a component type.");

        // Get component pool.
        ComponentPool<ComponentType>& pool = this->GetPool<ComponentType>();

        // Return iterator.
        return pool.Begin();
    }

    template<typename ComponentType>
    typename ComponentPool<ComponentType>::ComponentIterator ComponentSystem::End()
    {
        // Validate component type.
        static_assert(std::is_base_of<Component, ComponentType>::value, "Not a component type.");

        // Get component pool.
        ComponentPool<ComponentType>& pool = this->GetPool<ComponentType>();

        // Return iterator.
        return pool.End();
    }
}
