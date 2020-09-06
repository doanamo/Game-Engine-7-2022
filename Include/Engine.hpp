/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <Core/Core.hpp>
#include <Core/ServiceStorage.hpp>

/*
    Engine Root

    Main class that encapsulated all engine subsystems.
*/

namespace Engine
{
    class Root final : private Common::NonCopyable
    {
    public:
        struct CreateFromParams
        {
            float maxUpdateDelta = 1.0f;
        };

        enum class CreateErrors
        {
            InvalidArgument,
            FailedServiceCreation,
            FailedResourceLoading,
        };

        using CreateResult = Common::Result<std::unique_ptr<Root>, CreateErrors>;
        static CreateResult Create(const CreateFromParams& params);

        using ErrorCode = int;

        ~Root();

        ErrorCode Run();

        const Core::ServiceStorage& GetServices() const;

    private:
        Root();

        Common::Result<void, CreateErrors> CreateServices();
        Common::Result<void, CreateErrors> LoadDefaultResources();

        void ProcessFrame();

        Core::ServiceStorage m_services;
        float m_maxUpdateDelta = 1.0f;
    };
}
