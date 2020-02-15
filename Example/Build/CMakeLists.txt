#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

# Minimum required CMake version.
cmake_minimum_required(VERSION 3.12)

#
# Config
#

# Configurable variables.
set(PROJECT_NAME "GameEngine")
set(TARGET_NAME "Example")
set(ENABLE_TESTS ON)

# Configurable directories.
set(ENGINE_DIR "../..")
set(DEPLOY_DIR "../Deploy")

#
# Files
#

# List of source files.
set(SOURCE_FILES
    "Main.cpp"
    "Precompiled.hpp"
    "Precompiled.cpp"
    
    "Scenes/GameScene.hpp"
    "Scenes/GameScene.cpp"
)

# Prepend source directory to source file paths.
list(TRANSFORM SOURCE_FILES PREPEND "../Source/")

# Create missing files from template.
set(EMPTY_INCLUDE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../Source/Empty.hpp")
set(EMPTY_SOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../Source/Empty.cpp")

set(FILE_LIST ${SOURCE_FILES})

foreach(FILE_PATH ${FILE_LIST})
    set(FILE_PATH_ABSOLUTE "${CMAKE_CURRENT_SOURCE_DIR}/${FILE_PATH}")

    if(NOT EXISTS ${FILE_PATH_ABSOLUTE})
        get_filename_component(FILE_EXT ${FILE_PATH_ABSOLUTE} EXT)

        if(${FILE_EXT} STREQUAL ".hpp")
            configure_file(${EMPTY_INCLUDE_FILE} "${FILE_PATH_ABSOLUTE}")
        elseif(${FILE_EXT} STREQUAL ".cpp")
            configure_file(${EMPTY_INCLUDE_FILE} "${FILE_PATH_ABSOLUTE}")
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
target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../Source/")

# Write file pointing at game directory.
get_filename_component(ABSOLUTE_GAME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Deploy" ABSOLUTE)
file(WRITE "${CMAKE_BINARY_DIR}/GameDir.txt" ${ABSOLUTE_GAME_DIR})

#
# Platform
#

# Keep folder structure of include and source files.
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/.." FILES ${SOURCE_FILES})

# Set target as default project for Visual Studio.
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})
    
# Set main build directory as working directory.
set_target_properties(${TARGET_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

# Compiler specific settings.
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # Use precompiled header for quicker compilation of static headers.
    set_source_files_properties(${SOURCE_FILES} PROPERTIES COMPILE_FLAGS "/Yu\"Precompiled.hpp\"
        /Fp\"${CMAKE_CURRENT_BINARY_DIR}/Source/Precompiled-$(Configuration).pch\"")

    set_source_files_properties("../Source/Precompiled.cpp" PROPERTIES COMPILE_FLAGS "/Yc\"Precompiled.hpp\"
        /Fp\"${CMAKE_CURRENT_BINARY_DIR}/Source/Precompiled-$(Configuration).pch\"")
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
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/${ENGINE_DIR}/Build" "Engine")
target_link_libraries(${TARGET_NAME} "Engine")
