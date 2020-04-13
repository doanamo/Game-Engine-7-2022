/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <memory>

/*
    Resource Pool

    Manages an instance pool for a single type of a resource.
    See ResourceManager class for more context.

    void ExampleResourcePool()
    {
        // Create a resource pool instance.
        System::ResourcePool<Texture> texturePool;

        // Set a default resource that will be used as a 
        // fall back in case of a failed resource acquisition.
        std::shared_ptr<Texture> defaultTexture = std::make_shared<Texture>();
        texturePool->SetDefault(defaultTexture);

        // Create resources in a scope so they can be released when unused.
        {
            // Load a resource from the pool. Returns an existing resource if exist.
            // Resource type must provide Load(std::string, ...) method for the template.
            std::shared_ptr<const Texture> textureA = texture->Load("Data/Textures/checkerboard.png");

            // This will return the previously loaded resource.
            std::shared_ptr<const Texture> textureB = texture->Load("Data/Textures/checkerboard.png");
        }
        
        // Release resources that are no longer referenced.
        resourceManager.ReleaseUnused();
    }
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
    class ResourcePool : public ResourcePoolInterface, private NonCopyable
    {
    public:
        using ResourcePtr = std::shared_ptr<Type>;
        using ResourceList = std::unordered_map<std::string, ResourcePtr>;
        using ResourceListPair = typename ResourceList::value_type;

    public:
        ResourcePool() = default;
        ~ResourcePool();

        ResourcePool(ResourcePool&& other);
        ResourcePool& operator=(ResourcePool&& other);

        void SetDefault(std::shared_ptr<Type> resource);
        std::shared_ptr<Type> GetDefault() const;

        template<typename... Arguments>
        std::shared_ptr<Type> Acquire(std::string name, Arguments... arguments);

        void ReleaseUnused() override;
        void ReleaseAll();

    private:
        std::shared_ptr<Type> m_defaultResource;
        ResourceList m_resources;
    };

    template<typename Type>
    ResourcePool<Type>::~ResourcePool()
    {
        // Release all resources now.
        this->ReleaseAll();
    }

    template<typename Type>
    ResourcePool<Type>::ResourcePool(ResourcePool&& other) :
        ResourcePool<Type>()
    {
        *this = std::move(other);
    }

    template<typename Type>
    ResourcePool<Type>& ResourcePool<Type>::operator=(ResourcePool&& other)
    {
        std::swap(m_defaultResource, other.m_defaultResource);
        std::swap(m_resources, other.m_resources);

        return *this;
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
    std::shared_ptr<Type> ResourcePool<Type>::Acquire(std::string name, Arguments... arguments)
    {
        // Return an existing resource if loaded.
        auto it = m_resources.find(name);
        if(it != m_resources.end())
        {
            ASSERT(it->second != nullptr, "Found resource is null!");

            // Return found resource.
            return it->second;
        }

        // Create a new named resource instance.
        std::shared_ptr<Type> resource = std::make_shared<Type>();
        if(!resource->Initialize(std::forward<Arguments>(arguments)...))
            return m_defaultResource;

        // Add resource to the list.
        auto result = m_resources.emplace(name, std::move(resource));
        ASSERT(result.second, "Failed to emplace new resource in resource pool!");

        // Return the resource pointer.
        return result.first->second;
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
                // Retrieve the name to print it later.
                std::string name = it->first;

                // Print a log message.
                LOG_INFO("Releasing resource: \"{}\"", name);

                // Release the resource.
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
            // Retrieve the name to print it later.
            std::string name = it->first;

            // Print a log message.
            LOG_INFO("Releasing resource: \"{}\"", name);

            // Release the resource.
            it = m_resources.erase(it);
        }

        ASSERT(m_resources.empty(), "Resource pool is not empty after releasing all resources!");
    }
}
