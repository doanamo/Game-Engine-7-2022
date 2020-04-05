#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

cmake_minimum_required(VERSION 3.16)

#
# Library
#

project(Script)
add_library(Script STATIC
    "../../Include/Script/ScriptState.hpp"
    "Precompiled.hpp"
    "ScriptState.cpp"
)

target_compile_features(Script PUBLIC cxx_std_17)
target_include_directories(Script PUBLIC "../../Include")
target_precompile_headers(Script PRIVATE "Precompiled.hpp")

#
# Dependencies
#

if(NOT TARGET Common)
    add_subdirectory("../Common" "Common" EXCLUDE_FROM_ALL)
endif()

target_link_libraries(Script PRIVATE Common)

if(NOT TARGET System)
    add_subdirectory("../System" "System" EXCLUDE_FROM_ALL)
endif()

target_link_libraries(Script PRIVATE System)

#
# External
#

add_subdirectory("../../External/Lua-5.3.5" "External/Lua" EXCLUDE_FROM_ALL)
target_link_libraries(Script PUBLIC "lua_static")
set_property(TARGET "lua_static" PROPERTY PROJECT_LABEL "Lua")
set_property(TARGET "lua_static" PROPERTY FOLDER "External")
