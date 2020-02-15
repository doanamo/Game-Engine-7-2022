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
    // Resource pool interface.
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

        // Released unused resources.
        virtual void ReleaseUnused() = 0;
    };

    // Resource pool class.
    template<typename Type>
    class ResourcePool : public ResourcePoolInterface
    {
    public:
        // Type declarations.
        using ResourcePtr = std::shared_ptr<Type>;
        using ResourceList = std::unordered_map<std::string, ResourcePtr>;
        using ResourceListPair = typename ResourceList::value_type;

    public:
        ResourcePool();
        ~ResourcePool();

        // Disallow copying.
        ResourcePool(const ResourcePool& other) = delete;
        ResourcePool& operator=(const ResourcePool& other) = delete;

        // Move constructor and assignment.
        ResourcePool(ResourcePool&& other);
        ResourcePool& operator=(ResourcePool&& other);

        // Sets the default resource.
        // This resource will be returned when acquisition fails.
        void SetDefault(std::shared_ptr<Type> resource);

        // Returns the set default resource.
        std::shared_ptr<Type> GetDefault() const;

        // Acquires a resource and assigns it a name.
        // Name argument can also be a full path to a resource, which will ensure uniqueness.
        template<typename... Arguments>
        std::shared_ptr<Type> Acquire(std::string name, Arguments... arguments);

        // Releases unused resources.
        void ReleaseUnused() override;

        // Releases all resources.
        void ReleaseAll();

    private:
        // Default resource.
        std::shared_ptr<Type> m_defaultResource;

        // List of resources.
        ResourceList m_resources;
    };

    // Template definitions.
    template<typename Type>
    ResourcePool<Type>::ResourcePool() :
        m_defaultResource(nullptr)
    {
    }

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
        // Call the move assignment.
        *this = std::move(other);
    }

    template<typename Type>
    ResourcePool<Type>& ResourcePool<Type>::operator=(ResourcePool&& other)
    {
        // Swap class members.
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
                LOG_INFO() << "Releasing resource: \"" << name << "\"";

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
            LOG_INFO() << "Releasing resource: \"" << name << "\"";

            // Release the resource.
            it = m_resources.erase(it);
        }

        ASSERT(m_resources.empty(), "Resource pool is not empty after releasing all resources!");
    }
}
