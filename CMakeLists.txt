#
# Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

cmake_minimum_required(VERSION 3.21)
include("Build/CMakeShared.cmake")

#
# Project
#

project(GameEngine)
custom_cmake_setup()

#
# Sources
#

add_subdirectory("Source")
add_subdirectory("Example")
add_subdirectory("Tests")

# This is here because it must be called from root CMakeLists file.
# Otherwise tests will fail to be run when CTest is invoked.
enable_testing()
