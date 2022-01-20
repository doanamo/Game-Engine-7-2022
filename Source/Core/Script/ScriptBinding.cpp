/*
    Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Core/Script/ScriptState.hpp"
using namespace Core;

extern "C"
{
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

bool ScriptState::BindInterface(ScriptState& scriptState)
{
    // Register logging function.
    lua_pushcfunction(scriptState, LuaLog);
    lua_setglobal(scriptState, "Log");

    return true;
}
