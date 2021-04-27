/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <memory>

/*
    Service Storage

    Storage for services which can be accessed in generic way from other parts of engine.
*/

namespace Core
{
    class Service;

    class ServiceStorage
    {
    public:
        using ServicePtr = std::unique_ptr<Service>;
        using ServiceList = std::vector<ServicePtr>;
        using ServiceMap = std::unordered_map<Reflection::TypeIdentifier, Service*>;

    public:
        ServiceStorage();
        ~ServiceStorage();

        bool Provide(std::unique_ptr<Service>& service);
        Service* Locate(Reflection::TypeIdentifier serviceType) const;

        template<typename ServiceType>
        ServiceType* Locate() const;

    private:
        ServiceList m_serviceList;
        ServiceMap m_serviceMap;
    };

    template<typename ServiceType>
    ServiceType* ServiceStorage::Locate() const
    {
        return static_cast<ServiceType*>(
            Locate(Reflection::GetIdentifier<ServiceType>()));
    }
}
