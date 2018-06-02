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
project("Engine")
add_subdirectory("Source")

# Enable source folders in Visual Studio.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")
