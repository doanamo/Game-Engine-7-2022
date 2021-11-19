#
# Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

cmake_minimum_required(VERSION 3.16)

#
# External
#

set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory("../External/gtest" "GoogleTest")
set_property(TARGET "gtest" PROPERTY FOLDER "External")
set_property(TARGET "gtest_main" PROPERTY FOLDER "External")
include(GoogleTest)

#
# Tests
#

project(Tests)
enable_testing()
add_subdirectory(Common)
add_subdirectory(Reflection)
add_subdirectory(Game)
