/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Core/Precompiled.hpp"
#include "Core/ServiceStorage.hpp"
#include "Core/Service.hpp"
using namespace Core;

ServiceStorage::ServiceStorage() = default;
ServiceStorage::~ServiceStorage()
{
    // Destroy attached services in reverse order.
    for(auto it = m_serviceList.rbegin(); it != m_serviceList.rend(); ++it)
    {
        it->reset();
    }
}

bool ServiceStorage::Attach(std::unique_ptr<Service>&& service)
{
    if(service == nullptr)
    {
        LOG_WARNING("Attempted to provide null service to service storage!");
        return false;
    }

    const Reflection::TypeIdentifier serviceType = Reflection::GetIdentifier(service);
    if(const auto it = m_serviceMap.find(serviceType); it != m_serviceMap.end())
    {
        LOG_ERROR("Attempted to provide service \"{}\" that already exists in service storage!",
            Reflection::GetName(serviceType).GetString());
        return false;
    }

    if(!service->OnAttach(this))
    {
        LOG_ERROR("Failed to attach service \"{}\" to storage!",
            Reflection::GetName(serviceType).GetString());
        return false;
    }

    Service* providedService = m_serviceList.emplace_back(std::move(service)).get();
    auto [it, result] = m_serviceMap.emplace(serviceType, providedService);
    ASSERT(result, "Failed to emplace service in storage!");

    return true;
}

Service* ServiceStorage::Locate(const Reflection::TypeIdentifier serviceType) const
{
    if(const auto it = m_serviceMap.find(serviceType); it != m_serviceMap.end())
    {
        return it->second;
    }

    ASSERT(false, "Could not find service of type \"{}\"!",
        Reflection::GetName(serviceType).GetString());
    return nullptr;
}
