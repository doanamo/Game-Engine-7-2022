/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "System/ResourceManager.hpp"
using namespace System;

ResourceManager::ResourceManager() = default;
ResourceManager::~ResourceManager() = default;

GenericResult ResourceManager::Initialize()
{
    LOG("Initializing resource manager...");
    LOG_SCOPED_INDENT();

    // Setup initialization guard.
    VERIFY(!m_initialized, "Instance has already been initialized!");
    SCOPE_GUARD_IF(!m_initialized, this->Reset());

    // Success!
    m_initialized = true;
    return Success();
}

void ResourceManager::ReleaseUnused()
{
    ASSERT(m_initialized, "Resource manager has not been initialized!");

    // Release all unused resources.
    for(auto& pair : m_pools)
    {
        ASSERT(pair.second != nullptr, "Resource pool is null!");

        // Release unused resources from each pool.
        auto& pool = pair.second;
        pool->ReleaseUnused();
    }
}
