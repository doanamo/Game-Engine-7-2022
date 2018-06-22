#
# Copyright (c) 2018 Piotr Doan. All rights reserved.
#

# Specify minimum CMake version required.
cmake_minimum_required(VERSION 3.11.2)

#
# Source
#

# Source files that will partake in the compilation process.
set(SOURCE_FILES
    "Source/Main.cpp"
)

#
# Target
#

# Declare an executable target that depends on the engine library.
add_executable("Example" ${SOURCE_FILES})
target_link_libraries("Example" "Engine")
include_directories("./../Include")

# Set main build directory as the working directory.
set_target_properties("Example" PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

# Write a file pointing at extra mount directories.
get_filename_component(FILESYSTEM_MOUNT_DIR "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
file(WRITE "${CMAKE_BINARY_DIR}/MountDirectory.txt" "${FILESYSTEM_MOUNT_DIR}/")
