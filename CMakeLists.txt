#
# Copyright (c) 2018 Piotr Doan. All rights reserved.
#

# CMake version that was used to write this configuration file.
# This is important to set in order to establish policies for many commands.
cmake_minimum_required(VERSION 3.11.2)

#
# Project
#

# Define our project.
project("GameEngine")
add_subdirectory("Source")

# Include tests in the build process.
enable_testing()
add_subdirectory("Tests")

# Include example in the build process.
add_subdirectory("Example")

# Enable source folders in Visual Studio.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

# Set default project for Visual Studio.
set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT "Example")
