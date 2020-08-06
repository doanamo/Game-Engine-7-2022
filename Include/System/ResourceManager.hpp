/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>
#include <Core/ServiceStorage.hpp>
#include "System/ResourcePool.hpp"
#include "System/FileSystem.hpp"

/*
    Resource Manager

    Tracks resource references and releases them when no longer needed.
    Wraps multiple ResourcePool instances that can hold resources
    of different types in a single ResourceManager instance.
*/

namespace System
{
    class FileSystem;

    class ResourceManager final : private Common::NonCopyable
    {
    public:
        struct CreateFromParams
        {
            Core::ServiceStorage* services;
        };

        enum class CreateErrors
        {
            InvalidArgument,
        };

        using CreateResult = Common::Result<std::unique_ptr<ResourceManager>, CreateErrors>;
        static CreateResult Create(const CreateFromParams& params);

        using ResourcePoolPtr = std::unique_ptr<ResourcePoolInterface>;
        using ResourcePoolList = std::unordered_map<std::type_index, ResourcePoolPtr>;
        using ResourcePoolPair = typename ResourcePoolList::value_type;

    public:
        ~ResourceManager();

        template<typename Type>
        void SetDefault(std::unique_ptr<Type>&& resource);

        template<typename Type>
        void SetDefault(std::shared_ptr<Type> resource);

        template<typename Type>
        std::shared_ptr<Type> GetDefault() const;

        template<typename Type, typename... Arguments>
        typename ResourcePool<Type>::AcquireResult Acquire(
            std::filesystem::path path, Arguments... arguments);

        template<typename Type, typename... Arguments>
        typename ResourcePool<Type>::AcquireResult AcquireRelative(
            std::filesystem::path path, std::filesystem::path relative, Arguments... arguments);

        void ReleaseUnused();

    private:
        ResourceManager();

        template<typename Type>
        ResourcePool<Type>* CreatePool();

        template<typename Type>
        ResourcePool<Type>* GetPool();

    private:
        FileSystem* m_fileSystem;
        ResourcePoolList m_pools;
    };

    template<typename Type>
    void ResourceManager::SetDefault(std::unique_ptr<Type>&& resource)
    {
        // Construct shared pointer from unique pointer.
        this->SetDefault(std::shared_ptr<Type>(std::move(resource)));
    }

    template<typename Type>
    void ResourceManager::SetDefault(std::shared_ptr<Type> resource)
    {
        // Get resource pool.
        ResourcePool<Type>* pool = this->GetPool<Type>();
        ASSERT(pool != nullptr, "Could not retrieve resource pool!");

        // Set default resource.
        pool->SetDefault(std::move(resource));
    }

    template<typename Type>
    std::shared_ptr<Type> ResourceManager::GetDefault() const
    {
        // Get resource pool.
        ResourcePool<Type>* pool = this->GetPool<Type>();
        ASSERT(pool != nullptr, "Could not retrieve resource pool!");

        // Return default resource.
        return pool->GetDefault();
    }

    template<typename Type, typename... Arguments>
    typename ResourcePool<Type>::AcquireResult ResourceManager::Acquire(
        std::filesystem::path path, Arguments... arguments)
    {
        // Call relative acquisition method with empty relative path.
        return this->AcquireRelative<Type>(path, "", std::forward<Arguments>(arguments)...);
    }

    template<typename Type, typename... Arguments>
    typename ResourcePool<Type>::AcquireResult ResourceManager::AcquireRelative(
        std::filesystem::path path, std::filesystem::path relative, Arguments... arguments)
    {
        // Get resource pool.
        ResourcePool<Type>* pool = this->GetPool<Type>();
        ASSERT(pool != nullptr, "Could not retrieve resource pool!");

        // Resolve file path using relative path.
        auto resolvedPath = m_fileSystem->ResolvePath(path, relative);
        if(!resolvedPath)
        {
            return Common::Failure(pool->GetDefault());
        }

        // Convert to relative path (more optimal).
        std::filesystem::path relativePath = std::filesystem::relative(resolvedPath.Unwrap());

        // Delegate call to resource pool, which will then delegate it further to a new resource instance.
        return pool->Acquire(relativePath, std::forward<Arguments>(arguments)...);
    }

    template<typename Type>
    ResourcePool<Type>* ResourceManager::CreatePool()
    {
        // Create and add new resource pool.
        auto pool = std::make_unique<ResourcePool<Type>>();
        auto pair = ResourcePoolPair(typeid(Type), std::move(pool));
        auto result = m_pools.emplace(std::move(pair));
        ASSERT(result.second, "Could not emplace new resource pool!");

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
