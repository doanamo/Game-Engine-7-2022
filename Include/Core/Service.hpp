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
    class Service : private Common::NonCopyable
    {
        REFLECTION_ENABLE(Service)

    public:
        virtual ~Service() = default;

    protected:
        Service() = default;
    };
}

REFLECTION_TYPE(Core::Service)
