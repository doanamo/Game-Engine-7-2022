/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Common/Event/EventReceiver.hpp>
#include "Game/GameSystem.hpp"
#include "Game/EntityHandle.hpp"
#include "Game/ComponentPool.hpp"

/*
    Component System

    Manages component types and their instances.
*/

namespace Game
{
    class EntitySystem;
    class GameInstance;

    class ComponentSystem final : public GameSystem
    {
        REFLECTION_ENABLE(ComponentSystem, GameSystem)

    public:
        using ComponentPoolPtr = std::unique_ptr<ComponentPoolInterface>;
        using ComponentPoolList = std::unordered_map<std::type_index, ComponentPoolPtr>;
        using ComponentPoolPair = ComponentPoolList::value_type;

        enum class CreateComponentErrors
        {
            InvalidEntity,
            AlreadyExists,
            FailedInitialization,
        };

        template<typename ComponentType>
        using CreateComponentResult = Common::Result<ComponentType*, CreateComponentErrors>;

        enum class LookupComponentErrors
        {
            Missing,
        };

        template<typename ComponentType>
        using LookupComponentResult = Common::Result<ComponentType*, LookupComponentErrors>;

    public:
        ComponentSystem();
        ~ComponentSystem() override;

        template<typename ComponentType>
        CreateComponentResult<ComponentType> Create(EntityHandle handle);

        template<typename ComponentType>
        LookupComponentResult<ComponentType> Lookup(EntityHandle handle);

        template<typename ComponentType>
        ComponentPool<ComponentType>& GetPool();

        template<typename ComponentType>
        typename ComponentPool<ComponentType>::ComponentIterator Begin();

        template<typename ComponentType>
        typename ComponentPool<ComponentType>::ComponentIterator End();

        EntitySystem* GetEntitySystem() const
        {
            return m_entitySystem;
        }

    private:
        bool OnAttach(const GameSystemStorage& gameSystems) override;

        const EntityEntry* GetEntityEntry(EntityHandle handle) const;

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
    ComponentSystem::CreateComponentResult<ComponentType>
        ComponentSystem::Create(EntityHandle handle)
    {
        static_assert(std::is_base_of<Component, ComponentType>::value, "Not a component type.");

        // Retrieve entity entry to determine if handle is valid.
        const EntityEntry* entityEntry = GetEntityEntry(handle);
        if(!entityEntry)
        {
            LOG_WARNING("Attempted to create component for an invalid entity handle.");
            return Common::Failure(CreateComponentErrors::InvalidEntity);
        }

        // Create new component.
        ComponentPool<ComponentType>& pool = GetPool<ComponentType>();
        auto componentResult = pool.CreateComponent(handle);
        if(!componentResult)
        {
            switch(componentResult.UnwrapFailure())
            {
            default:
                ASSERT(false, "Unknown error result!");

            case ComponentPool<ComponentType>::CreateComponentErrors::AlreadyExists:
                return Common::Failure(CreateComponentErrors::AlreadyExists);
            }
        }

        ComponentType* component = componentResult.Unwrap();

        // Check if entity has already been created and has its components initialized.
        // If entity has already been created, initialize the component right away.
        if(entityEntry->flags & EntityFlags::Created)
        {
            if(!pool.InitializeComponent(handle))
            {
                ASSERT_EVALUATE(pool.DestroyComponent(handle), "Could not destroy component!");
                return Common::Failure(CreateComponentErrors::FailedInitialization);
            }
        }

        return Common::Success(component);
    }

    template<typename ComponentType>
    ComponentSystem::LookupComponentResult<ComponentType>
        ComponentSystem::Lookup(EntityHandle handle)
    {
        static_assert(std::is_base_of<Component, ComponentType>::value, "Not a component type.");

        // Lookup from component pool
        ComponentPool<ComponentType>& pool = GetPool<ComponentType>();
        auto componentResult = pool.LookupComponent(handle);
        if(!componentResult)
        {
            switch(componentResult.UnwrapFailure())
            {
            default:
                ASSERT(false, "Unknown error result!");

            case ComponentPool<ComponentType>::LookupComponentErrors::Missing:
                return Common::Failure(LookupComponentErrors::Missing);
            }
        }
        
        return Common::Success(componentResult.Unwrap());
    }

    template<typename ComponentType>
    bool ComponentSystem::Destroy(EntityHandle handle)
    {
        static_assert(std::is_base_of<Component, ComponentType>::value, "Not a component type.");

        // Get component pool and attempt to destroy component.
        ComponentPool<ComponentType>* pool = GetPool<ComponentType>();
        ASSERT(pool != nullptr, "Retrieved a null component pool!");
        return pool->DestroyComponent(handle);
    }

    template<typename ComponentType>
    ComponentPool<ComponentType>& ComponentSystem::GetPool()
    {
        static_assert(std::is_base_of<Component, ComponentType>::value, "Not a component type.");

        // Find pool by component type and if missing, create one.
        auto it = m_pools.find(typeid(ComponentType));
        if(it == m_pools.end())
        {
            auto* pool = CreatePool<ComponentType>();
            ASSERT(pool, "Failed to create component pool!");
            return *pool;
        }

        // Cast and return pointer that we already know is a component pool.
        auto* pool = reinterpret_cast<ComponentPool<ComponentType>*>(it->second.get());
        ASSERT(pool, "Component systems contains null component pool!");
        return *pool;
    }

    template<typename ComponentType>
    ComponentPool<ComponentType>* ComponentSystem::CreatePool()
    {
        static_assert(std::is_base_of<Component, ComponentType>::value, "Not a component type.");

        // Create and add pool to the collection.
        auto pool = std::make_unique<ComponentPool<ComponentType>>(this);
        auto [it, result] = m_pools.emplace(std::piecewise_construct,
            std::forward_as_tuple(typeid(ComponentType)),
            std::forward_as_tuple(std::move(pool))
        );

        ASSERT(result, "Failed to emplace new component pool type!");
        return reinterpret_cast<ComponentPool<ComponentType>*>(it->second.get());
    }

    template<typename ComponentType>
    typename ComponentPool<ComponentType>::ComponentIterator ComponentSystem::Begin()
    {
        static_assert(std::is_base_of<Component, ComponentType>::value, "Not a component type.");

        return GetPool<ComponentType>().Begin();
    }

    template<typename ComponentType>
    typename ComponentPool<ComponentType>::ComponentIterator ComponentSystem::End()
    {
        static_assert(std::is_base_of<Component, ComponentType>::value, "Not a component type.");

        return GetPool<ComponentType>().End();
    }
}

REFLECTION_TYPE(Game::ComponentSystem, Game::GameSystem)
