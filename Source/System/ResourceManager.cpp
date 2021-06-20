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

ResourceManager::CreateResult ResourceManager::Create()
{
    LOG("Creating resource manager...");
    LOG_SCOPED_INDENT();
    
    auto instance = std::unique_ptr<ResourceManager>(new ResourceManager());
    return Common::Success(std::move(instance));
}

bool ResourceManager::OnAttach(const Core::ServiceStorage* services)
{
    m_fileSystem = services->Locate<System::FileSystem>();
    if(!m_fileSystem)
    {
        LOG_ERROR("Failed to locate file system service!");
        return false;
    }

    return true;
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
