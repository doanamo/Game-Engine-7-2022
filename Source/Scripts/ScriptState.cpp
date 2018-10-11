/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Scripts/ScriptState.hpp"
#include "System/FileSystem.hpp"
#include "Engine.hpp"
using namespace Scripts;

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

ScriptState::ScriptState() :
    m_state(nullptr),
    m_initialized(false)
{
}

ScriptState::~ScriptState()
{
    if(m_state != nullptr)
    {
        lua_close(m_state);
    }
}

ScriptState::ScriptState(ScriptState&& other)
{
    // Call the move assignment operator.
    *this = std::move(other);
}

ScriptState& ScriptState::operator=(ScriptState&& other)
{
    // Swap class members.
    std::swap(m_state, other.m_state);
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool ScriptState::Initialize()
{
    LOG() << "Initializing script state..." << LOG_INDENT();

    // Check if state has already been initialized.
    VERIFY(!m_initialized, "Script state has already been initialized!");

    // Setup a cleanup guard.
    SCOPE_GUARD_IF(!m_initialized, *this = ScriptState());

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

bool ScriptState::Initialize(const LoadFromText& params)
{
    LOG() << "Loading script state from text..." << LOG_INDENT();

    // Call the main initialization methods.
    if(!this->Initialize())
        return false;

    // Setup a cleanup method.
    bool initialized = false;
    SCOPE_GUARD_IF(!initialized, *this = ScriptState());

    // Validate arguments.
    if(params.scriptText.empty())
    {
        LOG_ERROR() << "Invalid parameter - \"scriptText\" is empty!";
        return false;
    }

    // Parse the text string.
    if(luaL_dostring(m_state, params.scriptText.c_str()) != 0)
    {
        LOG_ERROR() << "Could not process text string!";
        this->PrintError();
        return false;
    }

    // Success!
    return initialized = true;
}

bool ScriptState::Initialize(const LoadFromFile& params)
{
    LOG() << "Loading script state from \"" << params.filePath << "\" file..." << LOG_INDENT();

    // Call the main initialization methods.
    if(!this->Initialize())
        return false;

    // Setup a cleanup method.
    bool initialized = false;
    SCOPE_GUARD_IF(!initialized, *this = ScriptState());

    // Validate arguments.
    if(params.engine == nullptr)
    {
        LOG_ERROR() << "Invalid parameter - \"engine\" is null!";
        return false;
    }

    if(params.filePath.empty())
    {
        LOG_ERROR() << "Invalid parameter - \"filePath\" is null!";
        return false;
    }

    // Resolve file path.
    std::string resolvedFilePath = params.engine->fileSystem.ResolvePath(params.filePath);

    // Parse the text file.
    if(luaL_dofile(m_state, resolvedFilePath.c_str()) != 0)
    {
        LOG_ERROR() << "Could not process text file!";
        this->PrintError();
        return false;
    }

    // Success!
    return initialized = true;
}

void ScriptState::PrintError()
{
    VERIFY(m_initialized, "Scripting state has not been initialized!");

    // Make sure that there is a string on top of the stack.
    ASSERT(lua_isstring(m_state, -1), "Expected a string!");

    // Print error string to the log.
    LOG_WARNING() << "Lua Error: " << lua_tostring(m_state, -1);

    // Pop error string from the stack.
    lua_pop(m_state, 1);
}

void ScriptState::CleanStack()
{
    VERIFY(m_initialized, "Scripting state has not been initialized!");

    // Discard remaining objects on the stack.
    int size = lua_gettop(m_state);

    if(size != 0)
    {
        LOG_DEBUG() << "Cleaning " << size << " remaining objects on the stack...";
        lua_settop(m_state, 0);
    }
}

bool ScriptState::CollectGarbage(bool singleStep)
{
    VERIFY(m_initialized, "Script state has not been initialized!");

    if(singleStep)
    {
        // Perform only one step of the garbage collection process.
        return lua_gc(m_state, LUA_GCSTEP, 0) == 0;
    }
    else
    {
        // Immediately collect all garbage at once.
        lua_gc(m_state, LUA_GCCOLLECT, 0);
        return false;
    }
}

bool ScriptState::IsValid() const
{
    return m_initialized;
}

ScriptState::operator lua_State*()
{
    VERIFY(m_initialized, "Script state has not been initialized!");

    return m_state;
}