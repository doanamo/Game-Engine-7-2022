#
# Copyright (c) 2018 Piotr Doan. All rights reserved.
#

# Specify minimum CMake version required.
cmake_minimum_required(VERSION 3.11.2)

#
# Source
#

# Files that take part in the build process.
set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/Include")
set(SOURCE_DIR "${PROJECT_SOURCE_DIR}/Source")
set(PRECOMPILED_HEADER "Precompiled")

set(INCLUDE_FILES
    "${INCLUDE_DIR}/Engine.hpp"

    "${INCLUDE_DIR}/Common/Build.hpp"
    "${INCLUDE_DIR}/Common/Debug.hpp"
    "${INCLUDE_DIR}/Common/NonCopyable.hpp"
    "${INCLUDE_DIR}/Common/ScopeGuard.hpp"
    "${INCLUDE_DIR}/Common/Utility.hpp"

    "${INCLUDE_DIR}/Logger/Logger.hpp"
    "${INCLUDE_DIR}/Logger/Message.hpp"
    "${INCLUDE_DIR}/Logger/Sink.hpp"
    "${INCLUDE_DIR}/Logger/Format.hpp"
    "${INCLUDE_DIR}/Logger/Output.hpp"
)

set(SOURCE_FILES
    "${SOURCE_DIR}/${PRECOMPILED_HEADER}.hpp"
    "${SOURCE_DIR}/${PRECOMPILED_HEADER}.cpp"

    "${SOURCE_DIR}/Common/Build.cpp"
    "${SOURCE_DIR}/Common/Debug.cpp"
    "${SOURCE_DIR}/Common/Utility.cpp"

    "${SOURCE_DIR}/Logger/Logger.cpp"
    "${SOURCE_DIR}/Logger/Message.cpp"
    "${SOURCE_DIR}/Logger/Sink.cpp"
    "${SOURCE_DIR}/Logger/Format.cpp"
    "${SOURCE_DIR}/Logger/Output.cpp"
)

# Keep folder structure of include and source files in Visual Studio.
source_group(TREE ${INCLUDE_DIR} PREFIX "Include Files" FILES ${INCLUDE_FILES})
source_group(TREE ${SOURCE_DIR} PREFIX "Source Files" FILES ${SOURCE_FILES})

#
# Target
#

# Declare a statically linked library target.
add_library("Engine" STATIC ${INCLUDE_FILES} ${SOURCE_FILES})
include_directories("./" ${INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

#
# Config
#

# Run Git commands to retrieve and save information about current build version.
find_package("Git")

add_custom_target("Configure"
    COMMAND ${CMAKE_COMMAND}
        -D BUILD_DIR="${CMAKE_BINARY_DIR}/"
        -D INCLUDE_DIR="${INCLUDE_DIR}/"
        -D SOURCE_DIR="${SOURCE_DIR}/"
        -D GIT_EXECUTABLE=${GIT_EXECUTABLE}
        -D INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/Config/BuildInfo.hpp.in"
        -D OUTPUT_FILE="${CMAKE_CURRENT_BINARY_DIR}/Config/BuildInfo.hpp"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/Config/BuildInfo.cmake"
)

add_dependencies("Engine" "Configure")

# Add file with generated build version to engine sources.
# Only the output header file is needed for the build process.
# We have to create a dummy file as it will not be generated yet.
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/Config/BuildInfo.hpp")

target_sources("Engine" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Config/BuildInfo.hpp.in")
source_group("Source Files\\Config" FILES "${CMAKE_CURRENT_SOURCE_DIR}/Config/BuildInfo.hpp.in")

target_sources("Engine" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Config/BuildInfo.cmake")
source_group("Source Files\\Config" FILES "${CMAKE_CURRENT_SOURCE_DIR}/Config/BuildInfo.cmake")

target_sources("Engine" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/Config/BuildInfo.hpp")
source_group("Source Files\\Config" FILES "${CMAKE_CURRENT_BINARY_DIR}/Config/BuildInfo.hpp")

#
# Platform
#

# Microsoft Visual Studio
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # Restore default main() entry instead of WinMain().
    set_property(TARGET "Engine" APPEND_STRING PROPERTY LINK_FLAGS "/ENTRY:mainCRTStartup ")

    # Disable Standard C++ Library warnings.
    set_property(TARGET "Engine" APPEND_STRING PROPERTY COMPILE_DEFINITIONS "_CRT_SECURE_NO_WARNINGS")
    set_property(TARGET "Engine" APPEND_STRING PROPERTY COMPILE_DEFINITIONS "_SCL_SECURE_NO_WARNINGS")
    
    # Use precompiled header for quicker compilation of static headers.
    set(PRECOMPILED_BINARY "Precompiled.pch")

    set_source_files_properties(${SOURCE_FILES} PROPERTIES
        COMPILE_FLAGS "/Yu\"${PRECOMPILED_HEADER}.hpp\" /Fp\"${PRECOMPILED_BINARY}\""
        OBJECT_DEPENDS "${PRECOMPILED_BINARY}"
    )

    set_source_files_properties("${PRECOMPILED_HEADER}.cpp" PROPERTIES
        COMPILE_FLAGS "/Yc\"${PRECOMPILED_HEADER}.hpp\" /Fp\"${PRECOMPILED_BINARY}\""
        OBJECT_DEPENDS "${PRECOMPILED_BINARY}"
    )
endif()
