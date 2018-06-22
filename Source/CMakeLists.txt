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

set(TARGET_FILES
    "Include/Engine.hpp"
    "Source/${PRECOMPILED_HEADER}.hpp"
    "Source/${PRECOMPILED_HEADER}.cpp"

    "Include/Common/Build.hpp"
    "Source/Common/Build.cpp"
    "Include/Common/Debug.hpp"
    "Source/Common/Debug.cpp"
    "Include/Common/NonCopyable.hpp"
    "Include/Common/ScopeGuard.hpp"
    "Include/Common/Utility.hpp"
    "Source/Common/Utility.cpp"

    "Include/Events/Delegate.hpp"
    "Include/Events/Collector.hpp"
    "Include/Events/Dispatcher.hpp"
    "Include/Events/Receiver.hpp"

    "Include/Logger/Logger.hpp"
    "Source/Logger/Logger.cpp"
    "Include/Logger/Message.hpp"
    "Source/Logger/Message.cpp"
    "Include/Logger/Sink.hpp"
    "Source/Logger/Sink.cpp"
    "Include/Logger/Format.hpp"
    "Source/Logger/Format.cpp"
    "Include/Logger/Output.hpp"
    "Source/Logger/Output.cpp"

    "Include/System/Platform.hpp"
    "Source/System/Platform.cpp"
    "Include/System/Window.hpp"
    "Source/System/Window.cpp"

    "Include/Graphics/Buffer.hpp"
    "Source/Graphics/Buffer.cpp"
    "Include/Graphics/InputLayout.hpp"
    "Source/Graphics/InputLayout.cpp"
    "Include/Graphics/Shader.hpp"
    "Source/Graphics/Shader.cpp"
)

append_directory_path("${PROJECT_SOURCE_DIR}" "TARGET_FILES")

# Keep folder structure of include and source files in Visual Studio.
source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${TARGET_FILES})

#
# Target
#

# Declare a statically linked library target.
add_library("Engine" STATIC ${TARGET_FILES})
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
source_group("Source\\Config" FILES "${CMAKE_CURRENT_SOURCE_DIR}/Config/BuildInfo.hpp.in")

target_sources("Engine" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Config/BuildInfo.cmake")
source_group("Source\\Config" FILES "${CMAKE_CURRENT_SOURCE_DIR}/Config/BuildInfo.cmake")

target_sources("Engine" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/Config/BuildInfo.hpp")
source_group("Source\\Config" FILES "${CMAKE_CURRENT_BINARY_DIR}/Config/BuildInfo.hpp")

# Write a file pointing at extra mount directories.
get_filename_component(FILESYSTEM_MOUNT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Data" ABSOLUTE)
file(WRITE "${CMAKE_BINARY_DIR}/MountDirectory.txt" "${FILESYSTEM_MOUNT_DIR}/")

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

# Add GLM library.
target_include_directories("Engine" PUBLIC "../External/GLM-0.9.9.0")

# Add OpenGL library.
find_package(OpenGL REQUIRED)
target_link_libraries("Engine" ${OPENGL_GL_LIBRARY})

# Add GLEW library.
set(BUILD_UTILS OFF CACHE BOOL "")

add_subdirectory("../External/GLEW-2.1.0/build/cmake" "GLEW" EXCLUDE_FROM_ALL)
target_include_directories("Engine" PUBLIC "../External/GLEW-2.1.0/include")
target_link_libraries("Engine" "glew_s")

set_property(TARGET "glew_s" PROPERTY PROJECT_LABEL "GLEW")
set_property(TARGET "glew_s" PROPERTY FOLDER "External")

# Add GLFW library.
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")

add_subdirectory("../External/GLFW-3.2.1" "GLFW" EXCLUDE_FROM_ALL)
target_include_directories("Engine" PUBLIC "../External/GLFW-3.2.1/include")
target_link_libraries("Engine" "glfw")

set_property(TARGET "glfw" PROPERTY PROJECT_LABEL "GLFW")
set_property(TARGET "glfw" PROPERTY FOLDER "External")
