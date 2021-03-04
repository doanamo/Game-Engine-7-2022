#
# Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Files
#

set(TEST_FILES
    "TestGame.cpp"
    "TestIdentitySystem.cpp"
)

#
# Library
#

project(TestGame)
add_executable(TestGame ${TEST_FILES})
target_compile_features(TestGame PUBLIC cxx_std_17)
add_test("Game" TestGame)

#
# Dependencies
#

add_subdirectory("../../Source/Core" "Core")
target_link_libraries(TestGame PRIVATE Core)

add_subdirectory("../../Source/Game" "Game")
target_link_libraries(TestGame PRIVATE Game)

#
# Environment
#

set_target_properties(TestGame PROPERTIES FOLDER "Tests")

#
# External
#

target_include_directories(TestGame PUBLIC "../../External/doctest")
