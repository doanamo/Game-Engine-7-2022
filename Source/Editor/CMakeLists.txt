#
# Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Files
#

set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/Include/Editor")
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

set(FILES_RESOURCES
    "${PROJECT_SOURCE_DIR}/Deploy/Data/Engine/Shaders/Interface.shader"
)

set(FILES_MODULES
    "${INCLUDE_DIR}/EditorModule.hpp"
    "${INCLUDE_DIR}/Modules/InputManagerEditor.hpp"
    "${INCLUDE_DIR}/Modules/GameInstanceEditor.hpp"
    "${SOURCE_DIR}/Modules/InputManagerEditor.cpp"
    "${SOURCE_DIR}/Modules/GameInstanceEditor.cpp"
)

set(FILES_SUBSYSTEMS
    "${INCLUDE_DIR}/EditorSubsystem.hpp"
    "${INCLUDE_DIR}/EditorShell.hpp"
    "${INCLUDE_DIR}/EditorConsole.hpp"
    "${INCLUDE_DIR}/EditorRenderer.hpp"
    "${SOURCE_DIR}/EditorShell.cpp"
    "${SOURCE_DIR}/EditorConsole.cpp"
    "${SOURCE_DIR}/EditorRenderer.cpp"
)

set(FILES_EDITOR
    "${INCLUDE_DIR}/EditorSystem.hpp"
    "${SOURCE_DIR}/EditorSystem.cpp"
)

source_group("Resources" FILES ${FILES_RESOURCES})
source_group("Modules" FILES ${FILES_MODULES})
source_group("Subsystems" FILES ${FILES_SUBSYSTEMS})
source_group("" FILES ${FILES_EDITOR})

#
# Library
#

add_library(Editor
    ${FILES_RESOURCES}
    ${FILES_MODULES}
    ${FILES_SUBSYSTEMS}
    ${FILES_EDITOR}
)

set_property(TARGET Editor PROPERTY FOLDER "Engine")
target_compile_features(Editor PUBLIC cxx_std_17)
target_include_directories(Editor PUBLIC
    "${PROJECT_SOURCE_DIR}/Include"
    "${PROJECT_SOURCE_DIR}/Source"
)

target_sources(Editor PRIVATE "${SOURCE_DIR}/Precompiled.hpp")
target_precompile_headers(Editor PRIVATE "${SOURCE_DIR}/Precompiled.hpp")
source_group("Generated\\Precompiled" REGULAR_EXPRESSION "cmake_pch")
source_group("" FILES "${SOURCE_DIR}/Precompiled.hpp")

#
# Dependencies
#

add_subdirectory("../Core" "Core")
target_link_libraries(Editor PRIVATE Core)

add_subdirectory("../System" "System")
target_link_libraries(Editor PRIVATE System)

add_subdirectory("../Game" "Game")
target_link_libraries(Editor PRIVATE Game)

enable_reflection(Editor ${INCLUDE_DIR} ${SOURCE_DIR})
