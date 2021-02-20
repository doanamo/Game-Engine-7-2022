#
# Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Files
#

set(INCLUDE_FILES
    "ScriptState.hpp"
)

list(TRANSFORM INCLUDE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/../../Include/Script/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/../../Include/Script/" PREFIX "Header Files" FILES ${INCLUDE_FILES})

set(SOURCE_FILES
    "Precompiled.hpp"
    "ScriptState.cpp"
)

list(TRANSFORM SOURCE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/" PREFIX "Source Files" FILES ${SOURCE_FILES})

#
# Library
#

project(Script)
add_library(Script ${INCLUDE_FILES} ${SOURCE_FILES})
target_compile_features(Script PUBLIC cxx_std_17)
target_include_directories(Script PUBLIC "../../Include" "../../Source")
target_precompile_headers(Script PRIVATE "Precompiled.hpp")
set_property(TARGET Script PROPERTY FOLDER "Engine")

#
# Dependencies
#

add_subdirectory("../Core" "Core")
target_link_libraries(Script PRIVATE Core)

add_subdirectory("../System" "System")
target_link_libraries(Script PRIVATE System)

#
# External
#

add_subdirectory("../../External/lua" "External/lua")
target_link_libraries(Script PUBLIC "lua_static")
set_property(TARGET "lua_static" PROPERTY PROJECT_LABEL "lua")
set_property(TARGET "lua_static" PROPERTY FOLDER "External")
