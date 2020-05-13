/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <memory>

/*
    Resource Pool

    Manages an instance pool for a single type of a resource.
    See ResourceManager class for more context.
*/

namespace System
{
    class ResourcePoolInterface
    {
    protected:
        ResourcePoolInterface()
        {
        }

    public:
        virtual ~ResourcePoolInterface()
        {
        }

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
        ResourcePool() = default;
        ~ResourcePool();

        void SetDefault(std::shared_ptr<Type> resource);
        std::shared_ptr<Type> GetDefault() const;

        template<typename... Arguments>
        AcquireResult Acquire(std::string name, Arguments... arguments);

        void ReleaseUnused() override;
        void ReleaseAll();

    private:
        std::shared_ptr<Type> m_defaultResource;
        ResourceList m_resources;
    };

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
    typename ResourcePool<Type>::AcquireResult ResourcePool<Type>::Acquire(std::string name, Arguments... arguments)
    {
        // Return existing resource if loaded.
        auto it = m_resources.find(name);
        if(it != m_resources.end())
        {
            ASSERT(it->second != nullptr, "Found resource is null!");

            // Return found resource.
            return Common::Success(it->second);
        }

        // Create resource instance.
        auto createResult = Type::Create(std::forward<Arguments>(arguments)...);
        if(!createResult)
        {
            return Common::Failure(m_defaultResource);
        }

        std::shared_ptr<Type> resource = createResult.Unwrap();
        ASSERT(resource != nullptr, "Successfully created resource is null!");

        // Add resource to the list.
        auto result = m_resources.emplace(name, std::move(resource));
        ASSERT(result.second, "Failed to emplace new resource in resource pool!");

        // Return resource pointer.
        return Common::Success(result.first->second);
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
            // Retrieve name to print it later.
            std::string name = it->first;

            // Release resource.
            LOG_INFO("Releasing resource: \"{}\"", name);
            it = m_resources.erase(it);
        }

        ASSERT(m_resources.empty(), "Resource pool is not empty after releasing all resources!");
    }
}
