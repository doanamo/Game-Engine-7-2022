/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <Core/Core.hpp>
#include <Core/ServiceStorage.hpp>

/*
    Engine Root
*/

namespace Engine
{
    class Root final : private Common::NonCopyable
    {
    public:
        struct CreateFromParams
        {
            float maxTickDelta = 1.0f;
        };

        enum class CreateErrors
        {
            InvalidArgument,
            FailedServiceCreation,
        };

        using CreateResult = Common::Result<std::unique_ptr<Root>, CreateErrors>;
        static CreateResult Create(const CreateFromParams& params);

    public:
        ~Root();

        int Run();

        const Core::ServiceStorage& GetServices() const;

    private:
        Root();

    private:
        Core::ServiceStorage m_services;
        float m_maxTickDelta = 1.0f;
    };
}
