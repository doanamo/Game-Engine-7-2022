/*
    Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Core/System/EngineSystem.hpp>

namespace Platform
{
    class FileHandle;
}

/*
    State

    Holds and manages Lua scripting state.
*/

namespace Core
{
    class ScriptState final : private Common::NonCopyable
    {
    public:
        struct LoadFromText
        {
            std::string scriptText;
        };

        struct LoadFromFile
        {
            const Core::EngineSystemStorage* engineSystems = nullptr;
        };

        enum class CreateErrors
        {
            InvalidArgument,
            FailedStateCreation,
            FailedLibraryBinding,
            FailedScriptExecution,
            FailedScriptFileResolve,
        };

        using CreateResult = Common::Result<std::unique_ptr<ScriptState>, CreateErrors>;

        static CreateResult Create();
        static CreateResult Create(Platform::FileHandle& file, const LoadFromFile& params);

    public:
        ~ScriptState();

        bool Execute(std::string script);

        void PrintError();
        void CleanStack();
        bool CollectGarbage(bool singleStep = false);

        operator lua_State*();

    private:
        ScriptState();

        bool BindInterface(ScriptState& scriptState);

    private:
        lua_State* m_state = nullptr;
    };
}
