#
# Copyright (c) 2018 Piotr Doan. All rights reserved.
#

# Specify minimum CMake version required.
cmake_minimum_required(VERSION 3.10)

#
# Config
#

# Project names.
set(PROJECT_NAME "GameEngine")
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
)

#
# Target
#

# Define project and executable target.
project(${PROJECT_NAME})
add_executable(${TARGET_NAME} ${SOURCE_FILES})

# Set main build directory as the working directory.
set_target_properties(${TARGET_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

# Write a file pointing at the working directory.
get_filename_component(ABSOLUTE_WORKING_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${WORKING_DIR}" ABSOLUTE)
file(WRITE "${CMAKE_BINARY_DIR}/WorkingDir.txt" "${ABSOLUTE_WORKING_DIR}/")

# Set default project for Visual Studio.
set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})

#
# Tests
#

# Enable testing.
if(${ENABLE_TESTS})
	enable_testing()
endif()

#
# External
#

# Link main dependency to the game engine.
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/${ENGINE_DIR}/" "Engine")
include_directories("${CMAKE_CURRENT_LIST_DIR}/${ENGINE_DIR}/Include/")
target_link_libraries(${TARGET_NAME} "Engine")
