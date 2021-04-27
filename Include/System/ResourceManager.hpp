/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Core/Service.hpp>
#include "System/ResourcePool.hpp"
#include "System/FileSystem/FileSystem.hpp"

namespace Core
{
    class ServiceStorage;
}

/*
    Resource Manager

    Tracks resource references and releases them when no longer needed.
    Wraps multiple ResourcePool instances that can hold resources of different
    types in a single ResourceManager instance.
*/

namespace System
{
    class FileSystem;

    class ResourceManager final : public Core::Service
    {
        REFLECTION_ENABLE(ResourceManager, Core::Service)

    public:
        struct CreateFromParams
        {
            Core::ServiceStorage* services = nullptr;
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
        ~ResourceManager() override;

        template<typename Type>
        void SetDefault(std::unique_ptr<Type>&& resource);

        template<typename Type>
        void SetDefault(std::shared_ptr<Type> resource);

        template<typename Type>
        std::shared_ptr<Type> GetDefault() const;

        template<typename Type, typename... Arguments>
        typename ResourcePool<Type>::AcquireResult Acquire(
            fs::path filePath, Arguments... arguments);

        template<typename Type, typename... Arguments>
        typename ResourcePool<Type>::AcquireResult AcquireRelative(
            fs::path filePath, fs::path relativeFilePath, Arguments... arguments);

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
        fs::path path, Arguments... arguments)
    {
        // Call relative acquisition method with empty relative path.
        return this->AcquireRelative<Type>(path, "",
            std::forward<Arguments>(arguments)...);
    }

    template<typename Type, typename... Arguments>
    typename ResourcePool<Type>::AcquireResult ResourceManager::AcquireRelative(
        fs::path path, fs::path relativePath, Arguments... arguments)
    {
        ResourcePool<Type>* pool = this->GetPool<Type>();
        ASSERT(pool != nullptr, "Could not retrieve resource pool!");
        return pool->Acquire(relativePath.remove_filename() / path,
            std::forward<Arguments>(arguments)...);
    }

    template<typename Type>
    ResourcePool<Type>* ResourceManager::CreatePool()
    {
        // Create and add new resource pool.
        auto pool = std::make_unique<ResourcePool<Type>>(m_fileSystem);
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

REFLECTION_TYPE(System::ResourceManager, Core::Service)
