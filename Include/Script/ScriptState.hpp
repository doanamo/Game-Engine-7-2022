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
    class ScriptState final : private NonCopyable, public Resettable<ScriptState>
    {
    public:
        enum class InitializeErrors
        {
            InvalidArgument,
            FailedLuaStateCreation,
            FailedLuaLibraryLoading,
            FailedLuaScriptExecution,
            FailedScriptFileResolve,
        };

        using InitializeResult = Result<void, InitializeErrors>;

        struct LoadFromText
        {
            std::string scriptText;
        };

        struct LoadFromFile
        {
            System::FileSystem* fileSystem = nullptr;
            std::string filePath;
        };

    public:
        ScriptState();
        ~ScriptState();

        InitializeResult Initialize();
        InitializeResult Initialize(const LoadFromText& params);
        InitializeResult Initialize(const LoadFromFile& params);
        bool IsInitialized() const;

        void PrintError();
        void CleanStack();
        bool CollectGarbage(bool singleStep);

        operator lua_State*();

    private:
        lua_State* m_state = nullptr;
        bool m_initialized = false;
    };
}
