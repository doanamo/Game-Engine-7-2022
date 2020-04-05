#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

cmake_minimum_required(VERSION 3.16)

#
# Library
#

project(Engine)
add_library(Engine STATIC
    "../Include/Engine.hpp"
    "Precompiled.hpp"
    "Engine.cpp"
)

target_compile_features(Engine PUBLIC cxx_std_17)
target_include_directories(Engine PUBLIC "../Include")
target_precompile_headers(Engine PRIVATE "Precompiled.hpp")

#
# Dependencies
#

if(NOT TARGET Common)
    add_subdirectory("Common")
endif()

target_link_libraries(Engine PUBLIC Common)

if(NOT TARGET Build)
    add_subdirectory("Build")
endif()

target_link_libraries(Engine PUBLIC Build)

if(NOT TARGET System)
    add_subdirectory("System")
endif()

target_link_libraries(Engine PUBLIC System)

if(NOT TARGET Script)
    add_subdirectory("Script")
endif()

target_link_libraries(Engine PUBLIC Script)

if(NOT TARGET Graphics)
    add_subdirectory("Graphics")
endif()

target_link_libraries(Engine PUBLIC Graphics)

if(NOT TARGET Game)
    add_subdirectory("Game")
endif()

target_link_libraries(Engine PUBLIC Game)

if(NOT TARGET Renderer)
    add_subdirectory("Renderer")
endif()

target_link_libraries(Engine PUBLIC Renderer)

if(NOT TARGET Editor)
    add_subdirectory("Editor")
endif()

target_link_libraries(Engine PUBLIC Editor)

#
# Environment
#

# Enable source folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

# Write file pointing at engine deploy directory.
get_filename_component(ABSOLUTE_ENGINE_DIR "../Deploy" ABSOLUTE)
file(WRITE "${CMAKE_BINARY_DIR}/EngineDir.txt" ${ABSOLUTE_ENGINE_DIR})
