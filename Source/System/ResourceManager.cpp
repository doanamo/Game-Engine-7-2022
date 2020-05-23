/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "System/ResourceManager.hpp"
using namespace System;

ResourceManager::ResourceManager() = default;
ResourceManager::~ResourceManager() = default;

ResourceManager::CreateResult ResourceManager::Create(const CreateFromParams& params)
{
    LOG("Creating resource manager...");
    LOG_SCOPED_INDENT();
    
    // Create instance.
    auto instance = std::unique_ptr<ResourceManager>(new ResourceManager());

    // Acquire file system.
    instance->m_fileSystem = params.services->GetFileSystem();
    if(!instance->m_fileSystem)
    {
        LOG_ERROR("Could not acquire file system service!");
        return Common::Failure(CreateErrors::FailedServiceAcquisition);
    }

    // Success!
    return Common::Success(std::move(instance));
}

void ResourceManager::ReleaseUnused()
{
    // Release all unused resources.
    for(auto& pair : m_pools)
    {
        ASSERT(pair.second != nullptr, "Resource pool is null!");

        // Release unused resources from each pool.
        auto& pool = pair.second;
        pool->ReleaseUnused();
    }
}
