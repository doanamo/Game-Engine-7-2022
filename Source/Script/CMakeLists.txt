#
# Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Files
#

set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/Include/Script")
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

set(FILES_SCRIPT
    "${INCLUDE_DIR}/ScriptState.hpp"
    "${SOURCE_DIR}/ScriptState.cpp"
)

source_group("" FILES ${FILES_SCRIPT})

#
# Library
#

add_library(Script
    ${FILES_SCRIPT}
)

set_property(TARGET Script PROPERTY FOLDER "Engine")
target_compile_features(Script PUBLIC cxx_std_17)
target_include_directories(Script PUBLIC
    "${PROJECT_SOURCE_DIR}/Include"
    "${PROJECT_SOURCE_DIR}/Source"
)

target_sources(Script PRIVATE "${SOURCE_DIR}/Precompiled.hpp")
target_precompile_headers(Script PRIVATE "${SOURCE_DIR}/Precompiled.hpp")
source_group("Generated\\Precompiled" REGULAR_EXPRESSION "cmake_pch")
source_group("" FILES "${SOURCE_DIR}/Precompiled.hpp")

#
# Dependencies
#

add_subdirectory("../Core" "Core")
target_link_libraries(Script PRIVATE Core)

add_subdirectory("../System" "System")
target_link_libraries(Script PRIVATE System)

enable_reflection(Script ${INCLUDE_DIR} ${SOURCE_DIR})

#
# External
#

add_subdirectory("../../External/lua" "External/lua")
target_link_libraries(Script PUBLIC "lua_static")
set_property(TARGET "lua_static" PROPERTY PROJECT_LABEL "lua")
set_property(TARGET "lua_static" PROPERTY FOLDER "External")
