/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <lua.hpp>
#include <Core/EngineSystem.hpp>

namespace System
{
    class FileHandle;
}

/*
    State

    Holds and manages Lua scripting state.
*/

namespace Script
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
            FailedLuaStateCreation,
            FailedLuaLibraryLoading,
            FailedLuaScriptExecution,
            FailedScriptFileResolve,
        };

        using CreateResult = Common::Result<std::unique_ptr<ScriptState>, CreateErrors>;

        static CreateResult Create();
        static CreateResult Create(const LoadFromText& params);
        static CreateResult Create(System::FileHandle& file, const LoadFromFile& params);

    public:
        ~ScriptState();

        void PrintError();
        void CleanStack();
        bool CollectGarbage(bool singleStep);

        operator lua_State*();

    private:
        ScriptState();

    private:
        lua_State* m_state = nullptr;
    };
}
