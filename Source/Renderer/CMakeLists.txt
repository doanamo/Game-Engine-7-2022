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
    "GameRenderer.hpp"
)

set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../Include/Renderer/")
list(TRANSFORM INCLUDE_FILES PREPEND ${INCLUDE_DIR})
source_group(TREE ${INCLUDE_DIR} PREFIX "Header Files" FILES ${INCLUDE_FILES})

set(SOURCE_FILES
    "Precompiled.hpp"
    "GameRenderer.cpp"
)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/")
list(TRANSFORM SOURCE_FILES PREPEND ${SOURCE_DIR})
source_group(TREE ${SOURCE_DIR} PREFIX "Source Files" FILES ${SOURCE_FILES})

#
# Library
#

project(Renderer)
add_library(Renderer ${INCLUDE_FILES} ${SOURCE_FILES})
target_compile_features(Renderer PUBLIC cxx_std_17)
target_include_directories(Renderer PUBLIC "../../Include" "../../Source")
target_precompile_headers(Renderer PRIVATE "Precompiled.hpp")
set_property(TARGET Renderer PROPERTY FOLDER "Engine")

#
# Dependencies
#

add_subdirectory("../Core" "Core")
target_link_libraries(Renderer PRIVATE Core)

add_subdirectory("../System" "System")
target_link_libraries(Renderer PRIVATE System)

add_subdirectory("../Graphics" "Graphics")
target_link_libraries(Renderer PRIVATE Graphics)

enable_reflection(Renderer ${INCLUDE_DIR} ${SOURCE_DIR})
