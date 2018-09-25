/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    Scripting State

    Holds and manages Lua scripting state.
*/

namespace Scripting
{
    // Scripting state class.
    class State
    {
    public:
        // Loads state from a text.
        struct LoadFromText
        {
            std::string scriptText;
        };

        // Loads state from a file.
        struct LoadFromFile
        {
            std::string filePath;
        };

    public:
        State();
        ~State();

        State(const State& other) = delete;
        State& operator=(const State& other) = delete;

        State(State&& other);
        State& operator=(State&& other);

        // Initializes the scripting state.
        bool Initialize();

        // Initializes the scripting state from text.
        bool Initialize(const LoadFromText& parameters);

        // Initializes the scripting state from file.
        bool Initialize(const LoadFromFile& parameters);

        // Prints and pops an error on the stack.
        void PrintError();

        // Discards remaining objects on the stack.
        void CleanStack();

        // Collects memory garbage.
        // Returns whether there is more garbage left to collect.
        bool CollectGarbage(bool singleStep);

    private:
        // Lua scripting state.
        lua_State* m_state;

        // Initialization state.
        bool m_initialized;
    };
}
