/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Event/Receiver.hpp"
#include "Game/EntitySystem.hpp"
#include "Game/ComponentPool.hpp"

/*
    Component System

    Manages component types and their instances.

    void ExampleEntitySystem()
    {
        // Declaring a component type.
        class Class : public Game::Component
        {
            // ...
        };

        // Adding components to entities.
        EntityHandle entity = entitySystem.CreateEntity();
        auto transform = componentSystem.Create<Game::Components::Transform>(entity);
        auto render = componentSystem.Create<Game::Components::Render>(entity);
        entitySystem.ProcessCommands();

        // Iterate over all components of a given type.
        auto componentsBegin = m_componentSystem->Begin<Components::Class>();
        auto componentsEnd = m_componentSystem->End<Components::Class>();
    
        for(auto it = componentsBegin; it != componentsEnd; ++it)
        {
            const EntityHandle& entity = it->first;
            Components::Class& component = it->second;
        
            // ...
        }
    }
*/

namespace Game
{
    // Forward declarations.
    class EntitySystem;

    // Component system class.
    class ComponentSystem
    {
    public:
        // ComponentType declarations.
        using ComponentPoolPtr = std::unique_ptr<ComponentPoolInterface>;
        using ComponentPoolList = std::unordered_map<std::type_index, ComponentPoolPtr>;
        using ComponentPoolPair = ComponentPoolList::value_type;

    public:
        ComponentSystem();
        ~ComponentSystem();

        // Disallow copying.
        ComponentSystem(const ComponentSystem& other) = delete;
        ComponentSystem& operator=(const ComponentSystem& other) = delete;

        // Move constructor and operator.
        ComponentSystem(ComponentSystem&& other);
        ComponentSystem& operator=(ComponentSystem&& other);

        // Initializes the component system.
        bool Initialize(EntitySystem* entitySystem);

        // Creates a component.
        template<typename ComponentType>
        ComponentType* Create(EntityHandle handle);

        // Lookups a component.
        template<typename ComponentType>
        ComponentType* Lookup(EntityHandle handle);

        // Gets a component pool.
        // Creates a new pool if needed.
        template<typename ComponentType>
        ComponentPool<ComponentType>& GetPool();

        // Gets the begin iterator.
        template<typename ComponentType>
        typename ComponentPool<ComponentType>::ComponentIterator Begin();

        // Gets the end iterator.
        template<typename ComponentType>
        typename ComponentPool<ComponentType>::ComponentIterator End();

        // Gets the entity system reference.
        EntitySystem* GetEntitySystem() const;

    private:
        // Destroys a component.
        // Cannot be called freely when a component is alive.
        template<typename ComponentType>
        bool Destroy(EntityHandle handle);

        // Creates a component type.
        template<typename ComponentType>
        ComponentPool<ComponentType>* CreatePool();

        // Called when an entity is about to be created.
        // Used to initialize components that entity contains.
        bool OnEntityCreate(EntityHandle handle);

        // Called when an entity is about to be destroyed.
        void OnEntityDestroy(EntityHandle handle);

    private:
        // Initialization state.
        bool m_initialized;

        // Entity system.
        EntitySystem* m_entitySystem;

        // Component pools.
        ComponentPoolList m_pools;

        // Event receivers.
        Event::Receiver<bool(EntityHandle)> m_entityCreate;
        Event::Receiver<void(EntityHandle)> m_entityDestroy;
    };

    // Template definitions.
    template<typename ComponentType>
    ComponentType* ComponentSystem::Create(EntityHandle handle)
    {
        ASSERT(m_initialized, "Component system instance is not initialized!");

        // Validate component type.
        static_assert(std::is_base_of<Component, ComponentType>::value, "Not a component type.");

        // Get the component pool.
        ComponentPool<ComponentType>& pool = this->GetPool<ComponentType>();

        // Create a new component.
        ComponentType* component = pool.CreateComponent(handle);

        if(component != nullptr)
        {
            // Check if entity has already been created and has its components initialized.
            // If entity has already been created, initialize the component right away.
            EntitySystem::HandleFlags::Type entityFlags = m_entitySystem->GetEntityFlags(handle);

            if(entityFlags & EntitySystem::HandleFlags::Created)
            {
                // Initialize the component.
                if(!pool.InitializeComponent(handle))
                {
                    // Destroy component if initialization fails.
                    bool destroyResult = pool.DestroyComponent(handle);
                    ASSERT(destroyResult, "Could not destroy a known component!");
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
        ASSERT(m_initialized, "Component system instance is not initialized!");

        // Validate component type.
        static_assert(std::is_base_of<Component, ComponentType>::value, "Not a component type.");

        // Get the component pool.
        ComponentPool<ComponentType>& pool = this->GetPool<ComponentType>();

        // Lookup and return the component.
        return pool.LookupComponent(handle);
    }

    template<typename ComponentType>
    bool ComponentSystem::Destroy(EntityHandle handle)
    {
        ASSERT(m_initialized, "Component system instance is not initialized!");

        // Validate component type.
        static_assert(std::is_base_of<Component, ComponentType>::value, "Not a component type.");

        // Get the component pool.
        ComponentPool<ComponentType>* pool = this->GetPool<ComponentType>();
        ASSERT(pool != nullptr, "Retrieved a null component pool!");

        // Destroy the component.
        return pool->DestroyComponent(handle);
    }

    template<typename ComponentType>
    ComponentPool<ComponentType>& ComponentSystem::GetPool()
    {
        ASSERT(m_initialized, "Component system instance is not initialized!");

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
        ASSERT(m_initialized, "Component system instance is not initialized!");

        // Validate component type.
        static_assert(std::is_base_of<Component, ComponentType>::value, "Not a component type.");

        // Create and add pool to the collection.
        auto pool = std::make_unique<ComponentPool<ComponentType>>(this);
        auto result = m_pools.emplace(std::piecewise_construct,
            std::forward_as_tuple(typeid(ComponentType)),
            std::forward_as_tuple(std::move(pool))
        );

        ASSERT(result.second == true, "Failed to insert new component pool type!");

        // Return the created pool.
        return reinterpret_cast<ComponentPool<ComponentType>*>(result.first->second.get());
    }

    template<typename ComponentType>
    typename ComponentPool<ComponentType>::ComponentIterator ComponentSystem::Begin()
    {
        ASSERT(m_initialized, "Component system instance is not initialized!");

        // Validate component type.
        static_assert(std::is_base_of<Component, ComponentType>::value, "Not a component type.");

        // Get the component pool.
        ComponentPool<ComponentType>& pool = this->GetPool<ComponentType>();

        // Return the iterator.
        return pool.Begin();
    }

    template<typename ComponentType>
    typename ComponentPool<ComponentType>::ComponentIterator ComponentSystem::End()
    {
        ASSERT(m_initialized, "Component system instance is not initialized!");

        // Validate component type.
        static_assert(std::is_base_of<Component, ComponentType>::value, "Not a component type.");

        // Get the component pool.
        ComponentPool<ComponentType>& pool = this->GetPool<ComponentType>();

        // Return the iterator.
        return pool.End();
    }
}
