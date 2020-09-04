#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Files
#

set(INCLUDE_FILES
    "Build.hpp"
)

list(TRANSFORM INCLUDE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/../../Include/Build/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/../../Include/Build/" PREFIX "Header Files" FILES ${INCLUDE_FILES})

set(SOURCE_FILES
    "Build.cpp"
    "BuildInfo.hpp.in"
    "BuildInfo.cmake"
)

list(TRANSFORM SOURCE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/" PREFIX "Source Files" FILES ${SOURCE_FILES})

#
# Library
#

project(Build)
add_library(Build ${INCLUDE_FILES} ${SOURCE_FILES})
target_compile_features(Build PUBLIC cxx_std_17)
target_include_directories(Build PUBLIC "../../Include" "../../Source")
set_property(TARGET Build PROPERTY FOLDER "Engine")

#
# Dependencies
#

add_subdirectory("../Core" "Core")
target_link_libraries(Build PRIVATE Core)

#
# Configuration
#

# Run Git commands to retrieve and save information about current build version.
find_package("Git")

add_custom_target("BuildInfo"
    COMMAND ${CMAKE_COMMAND}
        -D GIT_EXECUTABLE="${GIT_EXECUTABLE}"
        -D ENGINE_PROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../../"
        -D GAME_PROJECT_DIR="${CMAKE_SOURCE_DIR}"
        -D INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/BuildInfo.hpp.in"
        -D OUTPUT_FILE="${CMAKE_CURRENT_BINARY_DIR}/BuildInfo.hpp"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/BuildInfo.cmake"
)

add_dependencies(Build BuildInfo)
set_property(TARGET BuildInfo PROPERTY FOLDER "Generated")

# Add file with generated build version to engine sources.
# Only the output header file is needed for the build process.
# We have to create a dummy file as it will not be generated yet.
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/BuildInfo.hpp")
target_sources(Build PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/BuildInfo.hpp")
target_include_directories(Build PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
