#
# Copyright (c) 2018 Piotr Doan. All rights reserved.
#

# Specify minimum CMake version required.
cmake_minimum_required(VERSION 3.11.2)

#
# Source
#

# Define a function for appending paths to string lists.
function(append_directory_path path files)
    set("filesTemp" "")

    foreach("file" ${${files}})
        list(APPEND "filesTemp" "${path}/${file}")
    endforeach()

    set(${files} ${filesTemp} PARENT_SCOPE)
endfunction(append_directory_path)

# Files that take part in the build process.
set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/Include")
set(SOURCE_DIR "${PROJECT_SOURCE_DIR}/Source")
set(PRECOMPILED_HEADER "Precompiled")

set(INCLUDE_FILES
    "Engine.hpp"

    "Common/Build.hpp"
    "Common/Debug.hpp"
    "Common/NonCopyable.hpp"
    "Common/ScopeGuard.hpp"
    "Common/Utility.hpp"

    "Events/Delegate.hpp"
    "Events/Collector.hpp"

    "Logger/Logger.hpp"
    "Logger/Message.hpp"
    "Logger/Sink.hpp"
    "Logger/Format.hpp"
    "Logger/Output.hpp"

    "System/Platform.hpp"
)

append_directory_path(${INCLUDE_DIR} "INCLUDE_FILES")

set(SOURCE_FILES
    "${PRECOMPILED_HEADER}.hpp"
    "${PRECOMPILED_HEADER}.cpp"

    "Common/Build.cpp"
    "Common/Debug.cpp"
    "Common/Utility.cpp"

    "Logger/Logger.cpp"
    "Logger/Message.cpp"
    "Logger/Sink.cpp"
    "Logger/Format.cpp"
    "Logger/Output.cpp"

    "System/Platform.cpp"
)

append_directory_path(${SOURCE_DIR} "SOURCE_FILES")

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

#
# External
#

# Add GLFW library.
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")

add_subdirectory("../External/GLFW-3.2.1" "GLFW" EXCLUDE_FROM_ALL)
include_directories("../External/GLFW-3.2.1/include")
target_link_libraries("Engine" "glfw")

set_property(TARGET "glfw" PROPERTY PROJECT_LABEL "GLFW")
set_property(TARGET "glfw" PROPERTY FOLDER "External")
