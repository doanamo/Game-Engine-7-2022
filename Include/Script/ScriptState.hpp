/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <lua.hpp>
#include <string>

namespace System
{
    class FileSystem;
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
            System::FileSystem* fileSystem = nullptr;
            std::string filePath;
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
        static CreateResult Create(const LoadFromFile& params);

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
