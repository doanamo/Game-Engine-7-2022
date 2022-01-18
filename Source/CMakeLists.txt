#
# Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

include_guard(GLOBAL)

#
# Files
#

set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/Include/")
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

set(FILES_ENGINE
    "${INCLUDE_DIR}/Engine.hpp"
    "${SOURCE_DIR}/Engine.cpp"
)

source_group("" FILES ${FILES_ENGINE})

#
# Library
#

add_library(Engine
    ${FILES_ENGINE}
)

set_property(TARGET Engine PROPERTY FOLDER "Engine")
target_compile_features(Engine PUBLIC cxx_std_17)
target_include_directories(Engine PUBLIC
    "${PROJECT_SOURCE_DIR}/Include"
    "${PROJECT_SOURCE_DIR}/Source"
)

#
# Precompilled Header
#

target_sources(Engine PRIVATE "${SOURCE_DIR}/Precompiled.hpp")
target_precompile_headers(Engine PRIVATE "${SOURCE_DIR}/Precompiled.hpp")
source_group("Generated\\Precompiled" REGULAR_EXPRESSION "cmake_pch")
source_group("" FILES "${SOURCE_DIR}/Precompiled.hpp")

#
# Dependencies
#

add_subdirectory("Core" "Engine/Core")
target_link_libraries(Engine PUBLIC Core)

add_subdirectory("Platform" "Engine/Platform")
target_link_libraries(Engine PUBLIC Platform)

add_subdirectory("Graphics" "Engine/Graphics")
target_link_libraries(Engine PUBLIC Graphics)

add_subdirectory("Game" "Engine/Game")
target_link_libraries(Engine PUBLIC Game)

add_subdirectory("Renderer" "Engine/Renderer")
target_link_libraries(Engine PUBLIC Renderer)

add_subdirectory("Editor" "Engine/Editor")
target_link_libraries(Engine PUBLIC Editor)

enable_reflection(Engine)

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
endif()
