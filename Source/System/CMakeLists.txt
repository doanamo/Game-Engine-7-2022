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
    "Platform.hpp"
    "Timer.hpp"
    "Window.hpp"
    "InputDefinitions.hpp"
    "InputState.hpp"
    "InputManager.hpp"
    "ResourcePool.hpp"
    "ResourceManager.hpp"
    "Image.hpp"
    "FileSystem/FileSystem.hpp"
    "FileSystem/FileHandle.hpp"
    "FileSystem/FileDepot.hpp"
    "FileSystem/NativeFileHandle.hpp"
    "FileSystem/NativeFileDepot.hpp"
    "FileSystem/MemoryFileHandle.hpp"
    "FileSystem/MemoryFileDepot.hpp"
    "FileSystem/ArchiveFileHandle.hpp"
    "FileSystem/ArchiveFileDepot.hpp"
)

list(TRANSFORM INCLUDE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/../../Include/System/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/../../Include/System/" PREFIX "Header Files" FILES ${INCLUDE_FILES})

set(SOURCE_FILES
    "Precompiled.hpp"
    "Platform.cpp"
    "Timer.cpp"
    "Window.cpp"
    "InputDefinitions.cpp"
    "InputState.cpp"
    "InputManager.cpp"
    "ResourceManager.cpp"
    "Image.cpp"
    "FileSystem/FileSystem.cpp"
    "FileSystem/FileHandle.cpp"
    "FileSystem/NativeFileHandle.cpp"
    "FileSystem/NativeFileDepot.cpp"
    "FileSystem/MemoryFileHandle.cpp"
    "FileSystem/MemoryFileDepot.cpp"
    "FileSystem/ArchiveFileHandle.cpp"
    "FileSystem/ArchiveFileDepot.cpp"
)

list(TRANSFORM SOURCE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/" PREFIX "Source Files" FILES ${SOURCE_FILES})

#
# Library
#

project(System)
add_library(System ${INCLUDE_FILES} ${SOURCE_FILES})
target_compile_features(System PUBLIC cxx_std_17)
target_include_directories(System PUBLIC "../../Include" "../../Source")
target_precompile_headers(System PRIVATE "Precompiled.hpp")
set_property(TARGET System PROPERTY FOLDER "Engine")

#
# Dependencies
#

add_subdirectory("../Core" "Core")
target_link_libraries(System PRIVATE Core)
