/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include "System/FileSystem/FileSystem.hpp"
#include "System/FileSystem/FileHandle.hpp"

/*
    Resource Pool

    Manages an instance pool for a single type of resource.
    See ResourceManager class for more context.
*/

namespace System
{
    class ResourcePoolInterface
    {
    protected:
        ResourcePoolInterface() = default;

    public:
        virtual ~ResourcePoolInterface() = default;
        virtual void ReleaseUnused() = 0;
    };

    template<typename Type>
    class ResourcePool final : public ResourcePoolInterface, private Common::NonCopyable
    {
    public:
        using ResourcePtr = std::shared_ptr<Type>;
        using ResourceList = std::unordered_map<std::string, ResourcePtr>;
        using ResourceListPair = typename ResourceList::value_type;
        using AcquireResult = Common::Result<ResourcePtr, ResourcePtr>;

    public:
        ResourcePool(FileSystem* fileSystem);
        ~ResourcePool();

        void SetDefault(std::shared_ptr<Type> resource);
        std::shared_ptr<Type> GetDefault() const;

        template<typename... Arguments>
        AcquireResult Acquire(fs::path path, Arguments... arguments);

        void ReleaseUnused() override;
        void ReleaseAll();

    private:
        FileSystem* m_fileSystem;
        std::shared_ptr<Type> m_defaultResource;
        ResourceList m_resources;
    };

    template<typename Type>
    ResourcePool<Type>::ResourcePool(FileSystem* fileSystem) :
        m_fileSystem(fileSystem)
    {
        ASSERT(m_fileSystem, "Resource pool needs valid file system reference!");
    }

    template<typename Type>
    ResourcePool<Type>::~ResourcePool()
    {
        this->ReleaseAll();
    }

    template<typename Type>
    void ResourcePool<Type>::SetDefault(std::shared_ptr<Type> resource)
    {
        m_defaultResource = resource;
    }

    template<typename Type>
    std::shared_ptr<Type> ResourcePool<Type>::GetDefault() const
    {
        return m_defaultResource;
    }

    template<typename Type>
    template<typename... Arguments>
    typename ResourcePool<Type>::AcquireResult ResourcePool<Type>::Acquire(
        fs::path path, Arguments... arguments)
    {
        path = path.lexically_normal();
        std::string key = path.generic_string();

        // Return existing resource if loaded.
        auto it = m_resources.find(key);
        if(it != m_resources.end())
        {
            ASSERT(it->second != nullptr, "Found resource is null!");
            return Common::Success(it->second);
        }

        std::unique_ptr<FileHandle> fileHandle = m_fileSystem->OpenFile(
            path, FileHandle::OpenFlags::Read).UnwrapOr(nullptr);

        if(fileHandle == nullptr)
        {
            return Common::Failure(m_defaultResource);
        }

        // Create resource instance.
        if(auto resourceCreateResult = Type::Create(*fileHandle,
            std::forward<Arguments>(arguments)...))
        {
            std::shared_ptr<Type> resource = resourceCreateResult.Unwrap();
            ASSERT(resource != nullptr, "Successfully created resource is null!");

            auto result = m_resources.emplace(key, std::move(resource));
            ASSERT(result.second, "Failed to emplace new resource in resource pool!");

            return Common::Success(result.first->second);
        }
        else
        {
            return Common::Failure(m_defaultResource);
        }
    }

    template<typename Type>
    void ResourcePool<Type>::ReleaseUnused()
    {
        // Release unused resources.
        auto it = m_resources.begin();
        while(it != m_resources.end())
        {
            if(it->second.use_count() == 1)
            {
                // Release resource.
                LOG_INFO("Releasing resource: \"{}\"", it->first);
                it = m_resources.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    template<typename Type>
    void ResourcePool<Type>::ReleaseAll()
    {
        // Release all resources.
        auto it = m_resources.begin();
        while(it != m_resources.end())
        {
            // Release resource.
            LOG_INFO("Releasing resource: \"{}\"", it->first);
            it = m_resources.erase(it);
        }

        ASSERT(m_resources.empty(), "Resource pool is not empty after releasing all resources!");
    }
}
