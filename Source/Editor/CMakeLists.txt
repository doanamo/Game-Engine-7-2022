#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Files
#

set(INCLUDE_FILES
    "EditorSystem.hpp"
    "EditorShell.hpp"
    "EditorRenderer.hpp"
    "Modules/InputManagerEditor.hpp"
    "Modules/GameStateEditor.hpp"
)

list(TRANSFORM INCLUDE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/../../Include/Editor/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/../../Include/Editor/" PREFIX "Header Files" FILES ${INCLUDE_FILES})

set(SOURCE_FILES
    "Precompiled.hpp"
    "EditorSystem.cpp"
    "EditorShell.cpp"
    "EditorRenderer.cpp"
    "Modules/InputManagerEditor.cpp"
    "Modules/GameStateEditor.cpp"
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/" PREFIX "Source Files" FILES ${SOURCE_FILES})
list(TRANSFORM SOURCE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")

set(RESOURCE_FILES
    "Shaders/Interface.shader"
)

list(TRANSFORM RESOURCE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/../../Deploy/Data/Engine/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/../../Deploy/Data/Engine/" PREFIX "Resource Files" FILES ${RESOURCE_FILES})

#
# Library
#

project(Editor)
add_library(Editor STATIC ${INCLUDE_FILES} ${SOURCE_FILES} ${RESOURCE_FILES})
target_compile_features(Editor PUBLIC cxx_std_17)
target_include_directories(Editor PUBLIC "../../Include")
target_precompile_headers(Editor PRIVATE "Precompiled.hpp")

#
# Dependencies
#

add_subdirectory("../Core" "Core")
target_link_libraries(Editor PRIVATE Core)

add_subdirectory("../System" "System")
target_link_libraries(Editor PRIVATE System)

add_subdirectory("../Game" "Game")
target_link_libraries(Editor PRIVATE Game)
