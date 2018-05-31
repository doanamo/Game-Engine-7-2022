#
# Copyright (c) 2018 Piotr Doan. All rights reserved.
#

# CMake version that was used to write this configuration file.
cmake_minimum_required(VERSION 3.11.2)

#
# Config
#

# Build settings.
set(PROJECT_NAME "Project")
set(TARGET_NAME "Engine")

# Application settings.
set(WORKING_DIR "../")
set(SHOW_CONSOLE ON)

#
# Source
#

# Source files.
set(SOURCE_DIR "../Source")
set(PRECOMPILED_HEADER "Precompiled")

set(SOURCE_FILES
    "Main.cpp"

    "${PRECOMPILED_HEADER}.hpp"
    "${PRECOMPILED_HEADER}.cpp"
)

# Append source directory path to each source file.
message("-- Appending source directory path to source files...")

foreach(SOURCE_FILE ${SOURCE_FILES})
    list(APPEND SOURCE_PATHS "${SOURCE_DIR}/${SOURCE_FILE}")
    message("${SOURCE_FILE} => ${SOURCE_DIR}/${SOURCE_FILE}")
endforeach()

# Organize source files based on their directory structure.
message("-- Organizing source files by their directories...")

foreach(SOURCE_PATH ${SOURCE_PATHS})
    # Get relative path to a source file's directory.
    get_filename_component(SOURCE_PATH_DIR ${SOURCE_PATH} DIRECTORY)

    # Remove root path to source directory.
    string(REPLACE "${SOURCE_DIR}" "" SOURCE_PATH_DIR ${SOURCE_PATH_DIR})

    if(NOT ${SOURCE_PATH_DIR} STREQUAL "")
        # Normalize path delimeters as required by CMake's source groups (at least for Visual Studio).
        string(REPLACE "/" "\\" SOURCE_PATH_DIR ${SOURCE_PATH_DIR})

        # Add leading path delimeter for non empty paths.
        set(SOURCE_PATH_DIR "\\${SOURCE_PATH_DIR}")
    endif()

    # Add source file to a source group.
    source_group("Source${SOURCE_PATH_DIR}" FILES ${SOURCE_PATH})
    message("${SOURCE_PATH} => Source${SOURCE_PATH_DIR}")
endforeach()

#
# Build
#

# Define project.
project(${PROJECT_NAME})

# Define target.
add_executable(${TARGET_NAME} ${SOURCE_PATHS})

# Add source directory as an include directory by default.
include_directories(${SOURCE_DIR})

# Enable unicode supports.
add_definitions(-DUNICODE -D_UNICODE)

#
# Debug
#

# Write a file that will point the environment to the working directory.
get_filename_component(WORKING_DIR_PATH "${CMAKE_CURRENT_LIST_DIR}/${WORKING_DIR}" ABSOLUTE)
file(WRITE "${CMAKE_BINARY_DIR}/WorkingDir.txt" "${WORKING_DIR_PATH}/")

# Write a file that will point the environment to the source directory.
get_filename_component(SOURCE_DIR_PATH, "${CMAKE_CURRENT_LIST_DIR}/${SOURCE_DIR}" ABSOLUTE)
file(WRITE "${CMAKE_BINARY_DIR}/SourceDir.txt" "${SOURCE_DIR_PATH}/")

#
# Platform
#

# Enable language features.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform specific settings.
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # Restore default main entry.
    set_property(TARGET ${TARGET_NAME} APPEND_STRING PROPERTY LINK_FLAGS "/ENTRY:mainCRTStartup ")

    # Toggle console window visibility.
    if(SHOW_CONSOLE)
        set_property(TARGET ${TARGET_NAME} APPEND_STRING PROPERTY LINK_FLAGS "/SUBSYSTEM:Console ")
    else()
        set_property(TARGET ${TARGET_NAME} APPEND_STRING PROPERTY LINK_FLAGS "/SUBSYSTEM:Windows ")
    endif()

    # Disable unwanted compile warnings.
    set_property(TARGET ${TARGET_NAME} APPEND_STRING PROPERTY COMPILE_DEFINITIONS "_CRT_SECURE_NO_WARNINGS")
    set_property(TARGET ${TARGET_NAME} APPEND_STRING PROPERTY COMPILE_DEFINITIONS "_SCL_SECURE_NO_WARNINGS")

    # Enable source groups.
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    # Move predefines targets to a seperate folder.
    set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

    # Set default startup project.
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})

    # Use the prompiled header.
    set(PRECOMPILED_BINARY "$(IntDir)${PRECOMPILED_HEADER}.pch")

    set_source_files_properties(${SOURCE_PATHS} PROPERTIES
        COMPILE_FLAGS "/Yu\"${PRECOMPILED_HEADER}.hpp\" /Fp\"${PRECOMPILED_BINARY}\""
        OBJECT_DEPENDS "${PRECOMPILED_BINARY}"
    )

    set_source_files_properties("${SOURCE_DIR}/${PRECOMPILED_HEADER}.cpp" PROPERTIES
        COMPILE_FLAGS "/Yc\"${PRECOMPILED_HEADER}.hpp\" /Fp\"${PRECOMPILED_BINARY}\""
        OBJECT_DEPENDS "${PRECOMPILED_BINARY}"
    )
endif()
