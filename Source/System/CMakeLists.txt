#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

cmake_minimum_required(VERSION 3.16)
include("../CMakeShared.cmake")
include_guard(GLOBAL)

#
# Files
#

set(INCLUDE_FILES
    "Platform.hpp"
    "FileSystem.hpp"
    "Window.hpp"
    "Timer.hpp"
    "InputDefinitions.hpp"
    "InputManager.hpp"
    "ResourcePool.hpp"
    "ResourceManager.hpp"
)

list(TRANSFORM INCLUDE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/../../Include/System/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/../../Include/System/" PREFIX "Header Files" FILES ${INCLUDE_FILES})

set(SOURCE_FILES
    "Precompiled.hpp"
    "Platform.cpp"
    "FileSystem.cpp"
    "Window.cpp"
    "Timer.cpp"
    "InputDefinitions.cpp"
    "InputManager.cpp"
    "ResourceManager.cpp"
)

list(TRANSFORM SOURCE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/" PREFIX "Source Files" FILES ${SOURCE_FILES})

#
# Library
#

project(System)
add_library(System STATIC ${INCLUDE_FILES} ${SOURCE_FILES})
target_compile_features(System PUBLIC cxx_std_17)
target_include_directories(System PUBLIC "../../Include")
target_precompile_headers(System PRIVATE "Precompiled.hpp")
set_custom_compiler_options()

#
# Dependencies
#

add_subdirectory("../Core" "Core")
target_link_libraries(System PRIVATE Core)
