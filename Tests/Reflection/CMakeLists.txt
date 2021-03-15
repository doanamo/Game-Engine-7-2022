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
    "TestReflection.cpp"
    "TestReflectionHeader.hpp"
    "TestReflectionCrossUnit.cpp"
)

#
# Test
#

project(TestReflection)
add_executable(TestReflection ${TEST_FILES})
target_compile_features(TestReflection PUBLIC cxx_std_17)
add_test("Reflection" TestReflection)

#
# Dependencies
#

add_subdirectory("../../Source/Reflection" "Reflection")
target_link_libraries(TestReflection PRIVATE Reflection)

#
# Environment
#

set_target_properties(TestReflection PROPERTIES FOLDER "Tests")

#
# External
#

target_include_directories(TestReflection PUBLIC "../../External/doctest")
