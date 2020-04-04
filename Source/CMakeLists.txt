#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

cmake_minimum_required(VERSION 3.16)

#
# Library
#

project(GameEngine)
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

add_subdirectory("Editor")
target_link_libraries(Engine PUBLIC Editor)
target_link_libraries(Engine PUBLIC Renderer)
target_link_libraries(Engine PUBLIC Game)
target_link_libraries(Engine PUBLIC Graphics)
target_link_libraries(Engine PUBLIC Script)
target_link_libraries(Engine PUBLIC System)
target_link_libraries(Engine PUBLIC Common)

#
# Configuration
#

# Enable source folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

# Write file pointing at engine deploy directory.
get_filename_component(ABSOLUTE_ENGINE_DIR "../Deploy" ABSOLUTE)
file(WRITE "${CMAKE_BINARY_DIR}/EngineDir.txt" ${ABSOLUTE_ENGINE_DIR})

# Set target as default project for Visual Studio.
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Engine)
