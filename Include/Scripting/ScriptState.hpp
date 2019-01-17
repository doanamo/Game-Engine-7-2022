/*
    Copyright (c) 2018-2019 Piotr Doan. All rights reserved.
*/

#pragma once

// Forward declarations.
namespace Engine
{
    class Root;
}

/*
    Script State

    Holds and manages Lua scripting state.
*/

namespace Scripting
{
    // Scripting state class.
    class ScriptState
    {
    public:
        // Load script state from a text.
        struct LoadFromText
        {
            std::string scriptText;
        };

        // Load script state from a file.
        struct LoadFromFile
        {
            Engine::Root* engine;
            std::string filePath;
        };

    public:
        ScriptState();
        ~ScriptState();

        ScriptState(const ScriptState& other) = delete;
        ScriptState& operator=(const ScriptState& other) = delete;

        ScriptState(ScriptState&& other);
        ScriptState& operator=(ScriptState&& other);

        // Initializes the scripting state.
        bool Initialize();

        // Initializes the scripting state from text.
        bool Initialize(const LoadFromText& params);

        // Initializes the scripting state from file.
        bool Initialize(const LoadFromFile& params);

        // Prints and pops an error on the stack.
        void PrintError();

        // Discards remaining objects on the stack.
        void CleanStack();

        // Collects memory garbage.
        // Returns whether there is more garbage left to collect.
        bool CollectGarbage(bool singleStep);

        // Checks if instance is valid.
        bool IsValid() const;

        // Conversion operator.
        // Allows instance of this class to be used with Lua C functions.
        operator lua_State*();

    private:
        // Lua scripting state.
        lua_State* m_state;

        // Initialization state.
        bool m_initialized;
    };
}
