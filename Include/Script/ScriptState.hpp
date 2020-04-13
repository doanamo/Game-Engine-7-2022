/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <lua.hpp>
#include <string.h>

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
    class ScriptState : private NonCopyable
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

    public:
        ScriptState() = default;
        ~ScriptState();

        ScriptState(ScriptState&& other);
        ScriptState& operator=(ScriptState&& other);

        bool Initialize();
        bool Initialize(const LoadFromText& params);
        bool Initialize(const LoadFromFile& params);

        void PrintError();
        void CleanStack();
        bool CollectGarbage(bool singleStep);

        bool IsValid() const;

        operator lua_State*();

    private:
        lua_State* m_state = nullptr;
        bool m_initialized = false;
    };
}
