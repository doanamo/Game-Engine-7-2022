#
# Copyright (c) 2018 Piotr Doan. All rights reserved.
#

# CMake version that was used to write this configuration file.
# This is important to set in order to establish policies for many commands.
cmake_minimum_required(VERSION 3.10)

#
# Config
#

# Project names.
set(PROJECT_NAME "Example")
set(TARGET_NAME "Example")

# Project settings.
set(ENABLE_TESTS ON)

# Project directories.
set(WORKING_DIR "Deploy/")
set(ENGINE_DIR "../")

#
# Source
#

# List of source files.
set(SOURCE_FILES
    "Source/Main.cpp"
    "Source/Precompiled.hpp"
    "Source/Precompiled.cpp"
    
    "Source/Scenes/GameScene.hpp"
    "Source/Scenes/GameScene.cpp"
)

#
# Target
#

# Define project and executable target.
project(${PROJECT_NAME})
add_executable(${TARGET_NAME} ${SOURCE_FILES})

# Make source folder an include directory.
target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Source")

# Set main build directory as the working directory.
set_target_properties(${TARGET_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

# Write file pointing at the working directory.
get_filename_component(ABSOLUTE_GAME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${GAME_DIR}" ABSOLUTE)
file(WRITE "${CMAKE_BINARY_DIR}/GameDir.txt" "${ABSOLUTE_GAME_DIR}/")

#
# Platform
#

# Keep folder structure of include and source files in Visual Studio.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})

# Set default project for Visual Stuido.
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})

# Microsoft Visual Studio only directives.
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # Disable Standard C++ Library warnings.
    set_property(TARGET ${TARGET_NAME} APPEND_STRING PROPERTY COMPILE_DEFINITIONS "_CRT_SECURE_NO_WARNINGS")
    set_property(TARGET ${TARGET_NAME} APPEND_STRING PROPERTY COMPILE_DEFINITIONS "_SCL_SECURE_NO_WARNINGS")
    
    # Use precompiled header for quicker compilation of static headers.
    set_source_files_properties(${SOURCE_FILES} PROPERTIES
        COMPILE_FLAGS "/Yu\"Precompiled.hpp\" /Fp\"${CMAKE_CURRENT_BINARY_DIR}/Source/Precompiled-$(Configuration).pch\""
    )

    set_source_files_properties("Source/Precompiled.cpp" PROPERTIES
        COMPILE_FLAGS "/Yc\"Precompiled.hpp\" /Fp\"${CMAKE_CURRENT_BINARY_DIR}/Source/Precompiled-$(Configuration).pch\""
    )
endif()

#
# Tests
#

# Enable tests.
if(ENABLE_TESTS)
    enable_testing()
endif()

#
# External
#

# Link game engine library as the dependency.
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/${ENGINE_DIR}" "Engine")
target_link_libraries(${TARGET_NAME} "Engine")
