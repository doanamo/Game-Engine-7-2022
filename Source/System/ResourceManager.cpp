/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "System/Precompiled.hpp"
#include "System/ResourceManager.hpp"
#include <Core/ServiceStorage.hpp>
using namespace System;

ResourceManager::ResourceManager() = default;
ResourceManager::~ResourceManager() = default;

ResourceManager::CreateResult ResourceManager::Create(const CreateFromParams& params)
{
    LOG("Creating resource manager...");
    LOG_SCOPED_INDENT();
    
    // Validate arguments.
    CHECK_ARGUMENT_OR_RETURN(params.services != nullptr, Common::Failure(CreateErrors::InvalidArgument));

    // Create instance.
    auto instance = std::unique_ptr<ResourceManager>(new ResourceManager());

    // Acquire file system service.
    instance->m_fileSystem = params.services->Locate<System::FileSystem>();

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
