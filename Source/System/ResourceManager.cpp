/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "ResourceManager.hpp"
using namespace System;

ResourceManager::ResourceManager() :
    m_initialized(false)
{
}

ResourceManager::~ResourceManager()
{
}

ResourceManager::ResourceManager(ResourceManager&& other) :
    ResourceManager()
{
    // Call the move assignment.
    *this = std::move(other);
}

ResourceManager& ResourceManager::operator=(ResourceManager&& other)
{
    // Swap class members.
    std::swap(m_pools, other.m_pools);
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool ResourceManager::Initialize()
{
    LOG("Initializing resource manager...");
    LOG_SCOPED_INDENT();

    // Check if resource manager has already been initialized.
    VERIFY(!m_initialized, "Resource manager has already been initialized!");

    // Setup a cleanup guard.
    SCOPE_GUARD_IF(!m_initialized, *this = ResourceManager());

    // Success!
    return m_initialized = true;
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
