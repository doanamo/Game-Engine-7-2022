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

#
# Externals
#

if(NOT EMSCRIPTEN)
    add_subdirectory("../../External/zlib" "External/zlib" EXCLUDE_FROM_ALL)
    target_include_directories(System PUBLIC "../../External/zlib")
    target_include_directories(System PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/External/zlib")
    target_link_libraries(System PUBLIC "zlibstatic")
    set_property(TARGET "zlibstatic" PROPERTY PROJECT_LABEL "zlib")
    set_property(TARGET "zlibstatic" PROPERTY FOLDER "External")

    set(PNG_BUILD_ZLIB ON)
    add_subdirectory("../../External/libpng" "External/libpng")
    target_include_directories("png_static" PRIVATE "../../External/zlib")
    target_include_directories("png_static" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/External/zlib")
    target_link_libraries(System PRIVATE "png_static")
    set_property(TARGET "png_static" PROPERTY PROJECT_LABEL "libpng")
    set_property(TARGET "png_static" PROPERTY FOLDER "External")
    set_property(TARGET "genfiles" PROPERTY PROJECT_LABEL "libpngconf")
    set_property(TARGET "genfiles" PROPERTY FOLDER "External")

    add_subdirectory("../../External/glfw" "External/glfw")
    target_include_directories(System PRIVATE "../../External/glfw/include")
    target_link_libraries(System PRIVATE "glfw")
    set_property(TARGET "glfw" PROPERTY FOLDER "External")
else()
    target_compile_options(System PRIVATE "SHELL: -s USE_ZLIB=1")
    target_link_options(System PRIVATE "SHELL: -s USE_ZLIB=1")

    target_compile_options(System PRIVATE "SHELL: -s USE_LIBPNG=1")
    target_link_options(System PRIVATE "SHELL: -s USE_LIBPNG=1")

    target_compile_options(System PRIVATE "SHELL: -s USE_GLFW=3")
    target_link_options(System PRIVATE "SHELL: -s USE_GLFW=3")
endif()

add_subdirectory("../../External/glad/" "External/glad")
target_link_libraries(System PUBLIC "glad")
set_property(TARGET "glad" PROPERTY FOLDER "External")
