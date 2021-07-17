#
# Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Files
#

set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/Include/Build")
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

set(FILES_GENERATED
    "${SOURCE_DIR}/BuildInfo.hpp.in"
    "${SOURCE_DIR}/BuildInfo.cmake"
)

set(FILES_BUILD
    "${INCLUDE_DIR}/Build.hpp"
    "${SOURCE_DIR}/Build.cpp"
)

source_group("Generated" FILES ${FILES_GENERATED})
source_group("" FILES ${FILES_BUILD})

#
# Library
#

add_library(Build
    ${FILES_GENERATED}
    ${FILES_BUILD}
)

set_property(TARGET Build PROPERTY FOLDER "Engine")
target_compile_features(Build PUBLIC cxx_std_17)
target_include_directories(Build PUBLIC
    "${PROJECT_SOURCE_DIR}/Include"
    "${PROJECT_SOURCE_DIR}/Source"
)

#
# Dependencies
#

add_subdirectory("../Core" "Core")
target_link_libraries(Build PRIVATE Core)

#
# Configuration
#

find_package("Git")

add_custom_target(BuildInfo
    COMMAND ${CMAKE_COMMAND} -E echo "Generating build info header..."
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
# Create empty output file first otherwise CMake will complain.
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/BuildInfo.hpp")
source_group("Generated" FILES "${CMAKE_CURRENT_BINARY_DIR}/BuildInfo.hpp")
target_sources(Build PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/BuildInfo.hpp")
target_include_directories(Build PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
