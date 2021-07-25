/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Core/Core.hpp>
#include <Core/SystemStorage.hpp>
#include <Core/EngineSystem.hpp>
#include <Core/ConfigTypes.hpp>

/*
    Engine Root

    Main class that encapsulated all engine subsystems.
*/

namespace Engine
{
    class Root final : private Common::NonCopyable
    {
    public:
        using ConfigVariables = Core::ConfigVariableArray;

        enum class CreateErrors
        {
            InvalidArgument,
            FailedSystemCreation,
            FailedResourceLoading,
        };

        using CreateResult = Common::Result<std::unique_ptr<Root>, CreateErrors>;
        static CreateResult Create(const ConfigVariables& configVars);

        using ErrorCode = int;

    public:
        ~Root();

        ErrorCode Run();

        const Core::EngineSystemStorage& GetSystems() const
        {
            return m_engineSystems;
        }

    private:
        Root();

        Common::Result<void, CreateErrors> CreateEngineSystems(const ConfigVariables& configVars);
        Common::Result<void, CreateErrors> LoadDefaultResources();

        void ProcessFrame();

    private:
        Core::EngineSystemStorage m_engineSystems;
    };
}
