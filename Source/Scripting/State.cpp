/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Scripting/State.hpp"
using namespace Scripting;

namespace
{
    extern "C"
    {
        // Prints a string on the stack to the log stream.
        static int LuaLog(lua_State* L)
        {
            ASSERT(L != nullptr, "Lua state is null!");

            if(lua_isstring(L, -1))
            {
                LOG_INFO() << lua_tostring(L, -1);
            }

            return 0;
        }
    }
}

State::State() :
    m_state(nullptr),
    m_initialized(false)
{
}

State::~State()
{
    if(m_state != nullptr)
    {
        lua_close(m_state);
    }
}

State::State(State&& other)
{
    // Call the move assignment operator.
    *this = std::move(other);
}

State& State::operator=(State&& other)
{
    // Swap class members.
    std::swap(m_state, other.m_state);
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool State::Initialize()
{
    LOG() << "Initializing scripting state..." << LOG_INDENT();

    // Check if state has already been initialized.
    VERIFY(!m_initialized, "Scripting state has already been initialized!");

    // Setup a cleanup guard.
    SCOPE_GUARD_IF(!m_initialized, *this = State());

    // Create Lua state.
    m_state = luaL_newstate();

    if(m_state == nullptr)
    {
        LOG_ERROR() << "Could not create Lua state!";
        return false;
    }

    // Load the base library.
    lua_pushcfunction(m_state, luaopen_base);
    lua_pushstring(m_state, "");

    if(lua_pcall(m_state, 1, 0, 0) != 0)
    {
        LOG_ERROR() << "Could not load the base library!";
        this->PrintError();
        return false;
    }

    // Register the logging function.
    lua_pushcfunction(m_state, LuaLog);
    lua_setglobal(m_state, "Log");

    // Make sure that we did not leave anything on the stack.
    ASSERT(lua_gettop(m_state) == 0, "Lua stack is not empty!");

    // Success!
    return m_initialized = true;
}

bool State::Initialize(const LoadFromText& parameters)
{
    LOG() << "Loading scripting state from text..." << LOG_INDENT();

    // Call the main initialization methods.
    if(!this->Initialize())
        return false;

    // Setup a cleanup method.
    bool initialized = false;
    SCOPE_GUARD_IF(!initialized, *this = State());

    // Parse the text string.
    if(luaL_dostring(m_state, parameters.scriptText.c_str()) != 0)
    {
        LOG_ERROR() << "Could not process text string!";
        this->PrintError();
        return false;
    }

    // Success!
    return initialized = true;
}

bool State::Initialize(const LoadFromFile& parameters)
{
    LOG() << "Loading scripting state from \"" << parameters.filePath << "\" file..." << LOG_INDENT();

    // Call the main initialization methods.
    if(!this->Initialize())
        return false;

    // Setup a cleanup method.
    bool initialized = false;
    SCOPE_GUARD_IF(!initialized, *this = State());

    // Parse the text file.
    if(luaL_dofile(m_state, parameters.filePath.c_str()) != 0)
    {
        LOG_ERROR() << "Could not process text file!";
        this->PrintError();
        return false;
    }

    // Success!
    return initialized = true;
}

void State::PrintError()
{
    VERIFY(m_initialized, "Scripting state has not been initialized!");

    // Make sure that there is a string on top of the stack.
    ASSERT(lua_isstring(m_state, -1), "Expected a string!");

    // Print error string to the log.
    LOG_WARNING() << "Lua Error: " << lua_tostring(m_state, -1);

    // Pop error string from the stack.
    lua_pop(m_state, 1);
}
