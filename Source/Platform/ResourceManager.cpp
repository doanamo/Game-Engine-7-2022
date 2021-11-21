/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Platform/Precompiled.hpp"
#include "Platform/ResourceManager.hpp"
#include <Core/System/SystemStorage.hpp>
using namespace Platform;

namespace
{
    const char* LogAttachError = "Failed to attach resource manager! {}";
}

ResourceManager::ResourceManager() = default;
ResourceManager::~ResourceManager() = default;

bool ResourceManager::OnAttach(const Core::EngineSystemStorage& engineSystems)
{
    // Locate required engine systems.
    m_fileSystem = &engineSystems.Locate<Platform::FileSystem>();

    return true;
}

void ResourceManager::OnPreFrame()
{
     ReleaseUnused();
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
