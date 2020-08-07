#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Files
#

set(INCLUDE_FILES
    "GameRenderer.hpp"
)

list(TRANSFORM INCLUDE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/../../Include/Renderer/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/../../Include/Renderer/" PREFIX "Header Files" FILES ${INCLUDE_FILES})

set(SOURCE_FILES
    "Precompiled.hpp"
    "GameRenderer.cpp"
)

list(TRANSFORM SOURCE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/" PREFIX "Source Files" FILES ${SOURCE_FILES})

#
# Library
#

project(Renderer)
add_library(Renderer ${INCLUDE_FILES} ${SOURCE_FILES})
target_compile_features(Renderer PUBLIC cxx_std_17)
target_include_directories(Renderer PUBLIC "../../Include")
target_precompile_headers(Renderer PRIVATE "Precompiled.hpp")

#
# Dependencies
#

add_subdirectory("../Core" "Core")
target_link_libraries(Renderer PRIVATE Core)

add_subdirectory("../System" "System")
target_link_libraries(Renderer PRIVATE System)

add_subdirectory("../Graphics" "Graphics")
target_link_libraries(Renderer PRIVATE Graphics)
