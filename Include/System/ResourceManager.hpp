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
*/

namespace System
{
    class ResourceManager final : private Common::NonCopyable
    {
    public:
        using CreateResult = Common::Result<std::unique_ptr<ResourceManager>, void>;
        static CreateResult Create();

        using ResourcePoolPtr = std::unique_ptr<ResourcePoolInterface>;
        using ResourcePoolList = std::unordered_map<std::type_index, ResourcePoolPtr>;
        using ResourcePoolPair = typename ResourcePoolList::value_type;

    public:
        ~ResourceManager();

        template<typename Type>
        void SetDefault(std::shared_ptr<Type> resource);

        template<typename Type>
        std::shared_ptr<Type> GetDefault() const;

        template<typename Type, typename... Arguments>
        typename ResourcePool<Type>::AcquireResult Acquire(std::string name, Arguments... arguments);

        void ReleaseUnused();

    private:
        ResourceManager();

        template<typename Type>
        ResourcePool<Type>* CreatePool();

        template<typename Type>
        ResourcePool<Type>* GetPool();

    private:
        ResourcePoolList m_pools;
    };

    template<typename Type>
    void ResourceManager::SetDefault(std::shared_ptr<Type> resource)
    {
        // Get resource pool.
        ResourcePool<Type>* pool = this->GetPool<Type>();
        ASSERT(pool != nullptr, "Could not retrieve a resource pool!");

        // Set default resource.
        pool->SetDefault(std::move(resource));
    }

    template<typename Type>
    std::shared_ptr<Type> ResourceManager::GetDefault() const
    {
        // Get resource pool.
        ResourcePool<Type>* pool = this->GetPool<Type>();
        ASSERT(pool != nullptr, "Could not retrieve a resource pool!");

        // Return default resource.
        return pool->GetDefault();
    }

    template<typename Type, typename... Arguments>
    typename ResourcePool<Type>::AcquireResult ResourceManager::Acquire(std::string name, Arguments... arguments)
    {
        // Get resource pool.
        ResourcePool<Type>* pool = this->GetPool<Type>();
        ASSERT(pool != nullptr, "Could not retrieve a resource pool!");

        // Delegate call to the resource pool, which will then delegate it further to a new resource instance.
        return pool->Acquire(name, std::forward<Arguments>(arguments)...);
    }

    template<typename Type>
    ResourcePool<Type>* ResourceManager::CreatePool()
    {
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
