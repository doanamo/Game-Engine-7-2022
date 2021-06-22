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

bool ResourceManager::OnAttach(const Core::ServiceStorage* services)
{
    // Locate required services.
    m_fileSystem = services->Locate<System::FileSystem>();
    if(!m_fileSystem)
    {
        LOG_ERROR("Failed to locate file system service!");
        return false;
    }

    // Success!
    return true;
}

void ResourceManager::ReleaseUnused()
{
    // Release all unused resources from all pools.
    for(auto& pair : m_pools)
    {
        ASSERT(pair.second != nullptr, "Resource pool is null!");
        auto& pool = pair.second;
        pool->ReleaseUnused();
    }
}

void ResourceManager::ReleaseAll()
{
    // Release all resources from all pools.
    for(auto& pair : m_pools)
    {
        ASSERT(pair.second != nullptr, "Resource pool is null!");
        auto& pool = pair.second;
        pool->ReleaseAll();
    }
}
