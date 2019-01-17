#
# Copyright (c) 2018-2019 Piotr Doan. All rights reserved.
#

# Minimum required CMake version.
cmake_minimum_required(VERSION 3.10)

#
# Config
#

# Configurable variables.
set(PROJECT_NAME "GameEngine")
set(TARGET_NAME "Example")
set(ENABLE_TESTS ON)

# Configurable directories.
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

# Create missing source files from template.
foreach(SOURCE_FILE ${SOURCE_FILES})
    set(SOURCE_FILE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE}")

    if(NOT EXISTS ${SOURCE_FILE_PATH})
        get_filename_component(SOURCE_FILE_EXT ${SOURCE_FILE_PATH} EXT)

        if(${SOURCE_FILE_EXT} STREQUAL ".hpp")
            configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Source/Empty.hpp" "${SOURCE_FILE_PATH}")
        elseif(${SOURCE_FILE_EXT} STREQUAL ".cpp")
            configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Source/Empty.cpp" "${SOURCE_FILE_PATH}")
        endif()
    endif()
endforeach()

#
# Target
#

# Create project and target.
project(${PROJECT_NAME})
add_executable(${TARGET_NAME} ${SOURCE_FILES})

# Add include directories.
target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Source")

# Write file pointing at the working directory.
get_filename_component(ABSOLUTE_GAME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${GAME_DIR}" ABSOLUTE)
file(WRITE "${CMAKE_BINARY_DIR}/GameDir.txt" "${ABSOLUTE_GAME_DIR}/")

#
# Platform
#

# Keep folder structure of include and source files.
source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCE_FILES})

# Set target as default project for Visual Studio.
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})
    
# Set main build directory as the working directory.
set_target_properties(${TARGET_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

# Compiler specific settings.
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
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

# Enable unit tests.
if(ENABLE_TESTS)
    enable_testing()
endif()

#
# External
#

# Link game engine library as the dependency.
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/${ENGINE_DIR}" "Engine")
target_link_libraries(${TARGET_NAME} "Engine")
