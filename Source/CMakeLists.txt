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
    "Engine.hpp"
)

set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Include/")
list(TRANSFORM INCLUDE_FILES PREPEND ${INCLUDE_DIR})
source_group(TREE ${INCLUDE_DIR} PREFIX "Header Files" FILES ${INCLUDE_FILES})

set(SOURCE_FILES
    "Precompiled.hpp"
    "Engine.cpp"
)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/")
list(TRANSFORM SOURCE_FILES PREPEND ${SOURCE_DIR})
source_group(TREE ${SOURCE_DIR} PREFIX "Source Files" FILES ${SOURCE_FILES})

#
# Library
#

project(Engine)
add_library(Engine ${INCLUDE_FILES} ${SOURCE_FILES})
target_compile_features(Engine PUBLIC cxx_std_17)
target_include_directories(Engine PUBLIC "../Include" "../../Source")
target_precompile_headers(Engine PRIVATE "Precompiled.hpp")
set_property(TARGET Engine PROPERTY FOLDER "Engine")

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

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

get_filename_component(ABSOLUTE_ENGINE_DIR "../Deploy" ABSOLUTE)
file(WRITE "${CMAKE_BINARY_DIR}/EngineDir.txt" ${ABSOLUTE_ENGINE_DIR})

if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html" PARENT_SCOPE)

    target_link_options(Engine PUBLIC
        "SHELL: --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/../Deploy/Data@Data/")

    target_compile_options(Engine PUBLIC "$<$<CONFIG:DEBUG>:SHELL: -s SAFE_HEAP=1>")
    target_link_options(Engine PUBLIC "$<$<CONFIG:DEBUG>:SHELL: -s SAFE_HEAP=1>")
    target_compile_options(Engine PUBLIC "$<$<CONFIG:DEBUG>:--profiling-funcs>")
    target_link_options(Engine PUBLIC "$<$<CONFIG:DEBUG>:--profiling-funcs>")
endif()
