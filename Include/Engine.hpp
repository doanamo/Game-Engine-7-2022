/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Core/Core.hpp>
#include <Core/ServiceStorage.hpp>
#include <Core/Config.hpp>

/*
    Engine Root

    Main class that encapsulated all engine subsystems.
*/

namespace Engine
{
    class Root final : private Common::NonCopyable
    {
    public:
        enum class CreateErrors
        {
            InvalidArgument,
            FailedServiceCreation,
            FailedResourceLoading,
        };

        using CreateResult = Common::Result<std::unique_ptr<Root>, CreateErrors>;
        static CreateResult Create(const Core::Config::VariableArray& configVars);

        using ErrorCode = int;

    public:
        ~Root();

        ErrorCode Run();

        const Core::ServiceStorage& GetServices() const;

    private:
        Root();

        Common::Result<void, CreateErrors> CreateServices(
            const Core::Config::VariableArray& configVars);
        Common::Result<void, CreateErrors> LoadDefaultResources();

        void ProcessFrame();

        Core::ServiceStorage m_services;
        float m_maxUpdateDelta = 1.0f;
    };
}
