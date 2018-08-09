/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include <typeindex>
#include <unordered_map>
#include "ResourcePool.hpp"

/*
    Resource Manager

    Tracks resource references and releases them when no longer needed.
    Wraps multiple ResourcePool instances that can hold resources
    of different types in a single ResourceManager instance.

    void ExampleResourceManager()
    {
        // Create a resource manager instance.
        System::ResourceManager resourceManager;
        if(!resourceManager.Initialize())
            return;
    
        // Set a default resource that will be returned on failed acquisitions.
        resourceManager->SetDefault<Texture>(std::make_shared<Texture>());
    
        // Load different resources in a scope.
        {
            // Load a resource from the pool. Returns an existing resource if exist.
            auto floorTexture = resourceManager->Acquire<Texture("Data/Textures/floor.png");
            auto grassTexture = resourceManager->Acquire<Texture>("Data/Textures/grass.png");

            // Load a shader using the same ResourceManager instance.
            std::shared_ptr<Shader> shader = resourceManager->Load<Shader>("Data/Shader/shiny.png");
        }

        // Release resources that are no longer referenced.
        resourceManager.ReleaseUnused();
    }
*/

namespace System
{
    // Resource manager class.
    class ResourceManager
    {
    public:
        // Type declarations.
        using ResourcePoolPtr = std::unique_ptr<ResourcePoolInterface>;
        using ResourcePoolList = std::unordered_map<std::type_index, ResourcePoolPtr>;
        using ResourcePoolPair = typename ResourcePoolList::value_type;

    public:
        ResourceManager();
        ~ResourceManager();

        // Disallow copying.
        ResourceManager(const ResourceManager& other) = delete;
        ResourceManager& operator=(const ResourceManager& other) = delete;

        // Move constructor and assignment.
        ResourceManager(ResourceManager&& other);
        ResourceManager& operator=(ResourceManager&& other);

        // Initializes the resource manager.
        bool Initialize();

        // Sets the default resource.
        template<typename Type>
        void SetDefault(std::shared_ptr<Type> default);

        // Gets the default resource.
        template<typename Type>
        std::shared_ptr<Type> GetDefault() const;

        // Acquires a resource.
        // Name argument can also be a full path to a resource, which will ensure uniqueness..
        template<typename Type, typename... Arguments>
        std::shared_ptr<Type> Acquire(std::string name, Arguments... arguments);

        // Releases unused resources of all types.
        void ReleaseUnused();

    private:
        // Creates a resource pool for a specified type of resource.
        template<typename Type>
        ResourcePool<Type>* CreatePool();

        // Returns a resource pool for a specified type of resource.
        template<typename Type>
        ResourcePool<Type>* GetPool();

    private:
        // Resource pools for different types.
        ResourcePoolList m_pools;
        
        // Initialization state.
        bool m_initialized;
    };

    // Template definitions.
    template<typename Type>
    void ResourceManager::SetDefault(std::shared_ptr<Type> default)
    {
        ASSERT(m_initialized, "Resource manager has not been initialized!");

        // Get a resource pool.
        ResourcePool<Type>* pool = this->GetPool<Type>();
        ASSERT(pool != nullptr, "Could not retrieve a resource pool!");

        // Set the default resource.
        pool->SetDefault(default);
    }

    template<typename Type>
    std::shared_ptr<Type> ResourceManager::GetDefault() const
    {
        ASSERT(m_initialized, "Resource manager has not been initialized!");

        // Get a resource pool.
        ResourcePool<Type>* pool = this->GetPool<Type>();
        ASSERT(pool != nullptr, "Could not retrieve a resource pool!");

        // Return the default resource.
        return pool->GetDefault();
    }

    template<typename Type, typename... Arguments>
    std::shared_ptr<Type> ResourceManager::Acquire(std::string name, Arguments... arguments)
    {
        ASSERT(m_initialized, "Resource manager has not been initialized!");

        // Get a resource pool.
        ResourcePool<Type>* pool = this->GetPool<Type>();
        ASSERT(pool != nullptr, "Could not retrieve a resource pool!");

        // Delegate call to the resource pool, which will then delegate it further to a new resource instance.
        return pool->Acquire(name, std::forward<Arguments>(arguments)...);
    }

    template<typename Type>
    ResourcePool<Type>* ResourceManager::CreatePool()
    {
        ASSERT(m_initialized, "Resource manager has not been initialized!");

        // Create and add a new resource pool.
        auto pool = std::make_unique<ResourcePool<Type>>();
        auto pair = ResourcePoolPair(typeid(Type), std::move(pool));
        auto result = m_pools.emplace(std::move(pair));
        ASSERT(result.second, "Could not emplace a new resource pool!");

        // Return the created resource pool.
        return reinterpret_cast<ResourcePool<Type>*>(result.first->second.get());
    }

    template<typename Type>
    ResourcePool<Type>* ResourceManager::GetPool()
    {
        ASSERT(m_initialized, "Resource manager has not been initialized!");

        // Find a pool by resource type.
        auto it = m_pools.find(typeid(Type));
        if(it != m_pools.end())
        {
            // Cast and return the pointer that we already know is a resource pool.
            ASSERT(it->second.get() != nullptr, "Retrieved resource pool is null!");
            return reinterpret_cast<ResourcePool<Type>*>(it->second.get());
        }
        else
        {
            // Create and return a new resource pool.
            return this->CreatePool<Type>();
        }
    }
};
