/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Platform/FileSystem/FileSystem.hpp>
#include "Core/System/EngineSystem.hpp"
#include "Core/Resource/ResourcePool.hpp"

/*
    Resource Manager

    Tracks resource references and releases them when no longer needed. Wraps multiple ResourcePool
    instances that can hold resources of different types in a single ResourceManager instance.
*/

namespace Core
{
    class FileSystem;

    class ResourceManager final : public Core::EngineSystem
    {
        REFLECTION_ENABLE(ResourceManager, Core::EngineSystem)

    public:
        using ResourcePoolPtr = std::unique_ptr<ResourcePoolInterface>;
        using ResourcePoolList = std::unordered_map<Reflection::TypeIdentifier, ResourcePoolPtr>;
        using ResourcePoolPair = typename ResourcePoolList::value_type;

    public:
        ResourceManager();
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
        void ReleaseAll();

    private:
        bool OnAttach(const Core::EngineSystemStorage& engineSystems) override;
        void OnPreFrame() override;

        template<typename Type>
        ResourcePool<Type>* CreatePool();

        template<typename Type>
        ResourcePool<Type>* GetPool();

    private:
        Platform::FileSystem* m_fileSystem = nullptr;
        ResourcePoolList m_pools;
    };

    template<typename Type>
    void ResourceManager::SetDefault(std::unique_ptr<Type>&& resource)
    {
        // Construct shared pointer from unique pointer.
        SetDefault(std::shared_ptr<Type>(std::move(resource)));
    }

    template<typename Type>
    void ResourceManager::SetDefault(std::shared_ptr<Type> resource)
    {
        // Get resource pool.
        ResourcePool<Type>* pool = GetPool<Type>();
        ASSERT(pool != nullptr, "Could not retrieve resource pool!");

        // Set default resource.
        pool->SetDefault(std::move(resource));
    }

    template<typename Type>
    std::shared_ptr<Type> ResourceManager::GetDefault() const
    {
        // Get resource pool.
        ResourcePool<Type>* pool = GetPool<Type>();
        ASSERT(pool != nullptr, "Could not retrieve resource pool!");

        // Return default resource.
        return pool->GetDefault();
    }

    template<typename Type, typename... Arguments>
    typename ResourcePool<Type>::AcquireResult ResourceManager::Acquire(
        fs::path path, Arguments... arguments)
    {
        // Call relative acquisition method with empty relative path.
        return AcquireRelative<Type>(path, "",
            std::forward<Arguments>(arguments)...);
    }

    template<typename Type, typename... Arguments>
    typename ResourcePool<Type>::AcquireResult ResourceManager::AcquireRelative(
        fs::path path, fs::path relativePath, Arguments... arguments)
    {
        ResourcePool<Type>* pool = GetPool<Type>();
        ASSERT(pool != nullptr, "Could not retrieve resource pool!");
        return pool->Acquire(relativePath.remove_filename() / path,
            std::forward<Arguments>(arguments)...);
    }

    template<typename Type>
    ResourcePool<Type>* ResourceManager::CreatePool()
    {
        static_assert(Reflection::IsReflected<Type>(), "Resource type must be reflected!");

        // Create and add new resource pool.
        auto pool = std::make_unique<ResourcePool<Type>>(m_fileSystem);
        auto pair = ResourcePoolPair(Reflection::GetIdentifier<Type>(), std::move(pool));
        auto result = m_pools.emplace(std::move(pair));
        ASSERT(result.second, "Could not emplace new resource pool!");

        // Return created resource pool.
        return reinterpret_cast<ResourcePool<Type>*>(result.first->second.get());
    }

    template<typename Type>
    ResourcePool<Type>* ResourceManager::GetPool()
    {
        static_assert(Reflection::IsReflected<Type>(), "Resource type must be reflected!");

        // Find pool by resource type.
        auto it = m_pools.find(Reflection::GetIdentifier<Type>());
        if(it != m_pools.end())
        {
            // Cast and return the pointer that we already know is a resource pool.
            ASSERT(it->second.get() != nullptr, "Retrieved resource pool is null!");
            return reinterpret_cast<ResourcePool<Type>*>(it->second.get());
        }
        else
        {
            // Create and return new resource pool.
            return CreatePool<Type>();
        }
    }
};

REFLECTION_TYPE(Core::ResourceManager, Core::EngineSystem)
