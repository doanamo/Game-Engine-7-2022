#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Files
#

set(INCLUDE_FILES
    "Engine.hpp"
)

list(TRANSFORM INCLUDE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/../Include/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/../Include/" PREFIX "Header Files" FILES ${INCLUDE_FILES})

set(SOURCE_FILES
    "Precompiled.hpp"
    "Engine.cpp"
)

list(TRANSFORM SOURCE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/" PREFIX "Source Files" FILES ${SOURCE_FILES})

#
# Library
#

project(Engine)
add_library(Engine STATIC ${INCLUDE_FILES} ${SOURCE_FILES})
target_compile_features(Engine PUBLIC cxx_std_17)
target_include_directories(Engine PUBLIC "../Include")
target_precompile_headers(Engine PRIVATE "Precompiled.hpp")

#
# Dependencies
#

add_subdirectory("Core")
target_link_libraries(Engine PUBLIC Core)

add_subdirectory("Build")
target_link_libraries(Engine PUBLIC Build)

add_subdirectory("System")
target_link_libraries(Engine PUBLIC System)

add_subdirectory("Script")
target_link_libraries(Engine PUBLIC Script)

add_subdirectory("Graphics")
target_link_libraries(Engine PUBLIC Graphics)

add_subdirectory("Game")
target_link_libraries(Engine PUBLIC Game)

add_subdirectory("Renderer")
target_link_libraries(Engine PUBLIC Renderer)

add_subdirectory("Editor")
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
