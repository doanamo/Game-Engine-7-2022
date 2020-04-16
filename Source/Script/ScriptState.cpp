/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Script/ScriptState.hpp"
#include <System/FileSystem.hpp>
using namespace Script;

namespace
{
    extern "C"
    {
        // Prints string on top of stack to log stream.
        static int LuaLog(lua_State* L)
        {
            ASSERT(L != nullptr, "Lua state is null!");

            if(lua_isstring(L, -1))
            {
                LOG_INFO(lua_tostring(L, -1));
            }

            return 0;
        }
    }
}

ScriptState::ScriptState() = default;

ScriptState::~ScriptState()
{
    if(m_state != nullptr)
    {
        lua_close(m_state);
    }
}

ScriptState::InitializeResult ScriptState::Initialize()
{
    LOG("Initializing script state...");
    LOG_SCOPED_INDENT();

    // Setup initialization guard.
    VERIFY(!m_initialized, "Instance has already been initialized!");
    SCOPE_GUARD_IF(!m_initialized, this->Reset());

    // Create Lua state.
    m_state = luaL_newstate();

    if(m_state == nullptr)
    {
        LOG_ERROR("Could not create Lua state!");
        return Failure(InitializeErrors::FailedLuaStateCreation);
    }

    // Load base library.
    lua_pushcfunction(m_state, luaopen_base);
    lua_pushstring(m_state, "");

    if(lua_pcall(m_state, 1, 0, 0) != 0)
    {
        LOG_ERROR("Could not load base Lua library!");
        this->PrintError();
        return Failure(InitializeErrors::FailedLuaLibraryLoading);
    }

    // Register logging function.
    lua_pushcfunction(m_state, LuaLog);
    lua_setglobal(m_state, "Log");

    // Make sure that we did not leave anything on the stack.
    ASSERT(lua_gettop(m_state) == 0, "Lua stack is not empty!");

    // Success!
    m_initialized = true;
    return Success();
}

ScriptState::InitializeResult ScriptState::Initialize(const LoadFromText& params)
{
    LOG("Loading script state from text...");
    LOG_SCOPED_INDENT();

    // Call main initialization method.
    SUCCESS_OR_RETURN_RESULT(this->Initialize());

    // Setup initialization guard.
    bool initialized = false;
    SCOPE_GUARD_IF(!initialized, this->Reset());

    // Check arguments.
    CHECK_ARGUMENT_OR_RETURN(!params.scriptText.empty(), Failure(InitializeErrors::InvalidArgument));

    // Execute script text.
    if(luaL_dostring(m_state, params.scriptText.c_str()) != 0)
    {
        LOG_ERROR("Could not execute script!");
        this->PrintError();
        return Failure(InitializeErrors::FailedLuaScriptExecution);
    }

    // Success!
    initialized = true;
    return Success();
}

ScriptState::InitializeResult ScriptState::Initialize(const LoadFromFile& params)
{
    LOG("Loading script state from \"{}\" file...", params.filePath);
    LOG_SCOPED_INDENT();

    // Call main initialization method.
    SUCCESS_OR_RETURN_RESULT(this->Initialize());

    // Setup initialization guard.
    bool initialized = false;
    SCOPE_GUARD_IF(!initialized, this->Reset());

    // Validate arguments.
    CHECK_ARGUMENT_OR_RETURN(params.fileSystem != nullptr, Failure(InitializeErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(!params.filePath.empty(), Failure(InitializeErrors::InvalidArgument));

    // Resolve path to script file.
    auto resolvePathResult = params.fileSystem->ResolvePath(params.filePath);

    if(!resolvePathResult)
    {
        LOG_ERROR("Could not resolve file path!");
        return Failure(InitializeErrors::FailedScriptFileResolve);
    }

    // Execute script file.
    if(luaL_dofile(m_state, resolvePathResult.Unwrap().c_str()) != 0)
    {
        LOG_ERROR("Could not load script file!");
        this->PrintError();
        return Failure(InitializeErrors::FailedLuaScriptExecution);
    }

    // Success!
    initialized = true;
    return Success();
}

void ScriptState::PrintError()
{
    VERIFY(m_initialized, "Script state has not been initialized!");

    // Make sure that there is a string on top of the stack.
    ASSERT(lua_isstring(m_state, -1), "Expected a string!");

    // Print error string to log.
    LOG_DEBUG("Lua Error: {}", lua_tostring(m_state, -1));

    // Pop error string from stack.
    lua_pop(m_state, 1);
}

void ScriptState::CleanStack()
{
    VERIFY(m_initialized, "Script state has not been initialized!");

    // Discard remaining objects on the stack.
    int size = lua_gettop(m_state);

    if(size != 0)
    {
        LOG_DEBUG("Cleaning {} remaining objects on the stack...", size);
        lua_settop(m_state, 0);
    }
}

bool ScriptState::CollectGarbage(bool singleStep)
{
    VERIFY(m_initialized, "Script state has not been initialized!");

    if(singleStep)
    {
        // Perform only one step of the garbage collection process.
        // Return whether there is more garbage left to collect.
        return lua_gc(m_state, LUA_GCSTEP, 0) == 0;
    }
    else
    {
        // Immediately collect all garbage at once.
        lua_gc(m_state, LUA_GCCOLLECT, 0);
        return false;
    }
}

bool ScriptState::IsInitialized() const
{
    return m_initialized;
}

ScriptState::operator lua_State*()
{
    VERIFY(m_initialized, "Script state has not been initialized!");
    return m_state;
}
