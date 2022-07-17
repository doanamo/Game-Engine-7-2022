#
# Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

cmake_minimum_required(VERSION 3.21)

#
# Options
#

set(ENGINE_DIR "..")
set(PROJECT_NAME Example)
set(TARGET_NAME Example)

include("${ENGINE_DIR}/Build/CMakeShared.cmake")

#
# Project
#

project(${PROJECT_NAME})
custom_cmake_setup()

#
# Sources
#

add_subdirectory("Source")
