/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <memory>
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
    class ResourceManager final : private NonCopyable, public Resettable<ResourceManager>
    {
    public:
        using ResourcePoolPtr = std::unique_ptr<ResourcePoolInterface>;
        using ResourcePoolList = std::unordered_map<std::type_index, ResourcePoolPtr>;
        using ResourcePoolPair = typename ResourcePoolList::value_type;

    public:
        ResourceManager();
        ~ResourceManager();

        GenericResult Initialize();

        template<typename Type>
        void SetDefault(std::shared_ptr<Type> resource);

        template<typename Type>
        std::shared_ptr<Type> GetDefault() const;

        template<typename Type, typename... Arguments>
        std::shared_ptr<Type> Acquire(std::string name, Arguments... arguments);

        void ReleaseUnused();

    private:
        template<typename Type>
        ResourcePool<Type>* CreatePool();

        template<typename Type>
        ResourcePool<Type>* GetPool();

    private:
        ResourcePoolList m_pools;
        bool m_initialized = false;
    };

    template<typename Type>
    void ResourceManager::SetDefault(std::shared_ptr<Type> resource)
    {
        ASSERT(m_initialized, "Resource manager has not been initialized!");

        // Get resource pool.
        ResourcePool<Type>* pool = this->GetPool<Type>();
        ASSERT(pool != nullptr, "Could not retrieve a resource pool!");

        // Set default resource.
        pool->SetDefault(resource);
    }

    template<typename Type>
    std::shared_ptr<Type> ResourceManager::GetDefault() const
    {
        ASSERT(m_initialized, "Resource manager has not been initialized!");

        // Get resource pool.
        ResourcePool<Type>* pool = this->GetPool<Type>();
        ASSERT(pool != nullptr, "Could not retrieve a resource pool!");

        // Return default resource.
        return pool->GetDefault();
    }

    template<typename Type, typename... Arguments>
    std::shared_ptr<Type> ResourceManager::Acquire(std::string name, Arguments... arguments)
    {
        ASSERT(m_initialized, "Resource manager has not been initialized!");

        // Get resource pool.
        ResourcePool<Type>* pool = this->GetPool<Type>();
        ASSERT(pool != nullptr, "Could not retrieve a resource pool!");

        // Delegate call to the resource pool, which will then delegate it further to a new resource instance.
        return pool->Acquire(name, std::forward<Arguments>(arguments)...);
    }

    template<typename Type>
    ResourcePool<Type>* ResourceManager::CreatePool()
    {
        ASSERT(m_initialized, "Resource manager has not been initialized!");

        // Create and add new resource pool.
        auto pool = std::make_unique<ResourcePool<Type>>();
        auto pair = ResourcePoolPair(typeid(Type), std::move(pool));
        auto result = m_pools.emplace(std::move(pair));
        ASSERT(result.second, "Could not emplace a new resource pool!");

        // Return created resource pool.
        return reinterpret_cast<ResourcePool<Type>*>(result.first->second.get());
    }

    template<typename Type>
    ResourcePool<Type>* ResourceManager::GetPool()
    {
        ASSERT(m_initialized, "Resource manager has not been initialized!");

        // Find pool by resource type.
        auto it = m_pools.find(typeid(Type));
        if(it != m_pools.end())
        {
            // Cast and return the pointer that we already know is a resource pool.
            ASSERT(it->second.get() != nullptr, "Retrieved resource pool is null!");
            return reinterpret_cast<ResourcePool<Type>*>(it->second.get());
        }
        else
        {
            // Create and return new resource pool.
            return this->CreatePool<Type>();
        }
    }
};
