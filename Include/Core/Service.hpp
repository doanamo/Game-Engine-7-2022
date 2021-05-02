/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

/*
    Service

    Base class for engine services that can be added to service storage .
*/

namespace Core
{
    class ServiceStorage;

    class Service : private Common::NonCopyable
    {
        REFLECTION_ENABLE(Service)

    public:
        virtual ~Service() = default;

    protected:
        friend ServiceStorage;

        Service() = default;

        virtual bool OnAttach(const ServiceStorage* serviceStorage)
        {
            return true;
        }
    };
}

REFLECTION_TYPE(Core::Service)
