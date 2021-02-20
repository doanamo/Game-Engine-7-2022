#
# Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Library
#

project(TestIdentitySystem)
add_executable(TestIdentitySystem "TestIdentitySystem.cpp")
target_compile_features(TestIdentitySystem PUBLIC cxx_std_17)
add_test("IdentitySystem" TestIdentitySystem)

#
# Dependencies
#

add_subdirectory("../Helpers" "TestHelpers")
target_link_libraries(TestIdentitySystem PRIVATE TestHelpers)

add_subdirectory("../../Source/Game" "Game")
target_link_libraries(TestIdentitySystem PRIVATE Game)

#
# Environment
#

set_target_properties(TestIdentitySystem PROPERTIES FOLDER "Tests")
