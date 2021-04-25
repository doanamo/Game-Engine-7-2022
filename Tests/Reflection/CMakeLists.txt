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
    "TestReflectionStatic.cpp"
    "TestReflectionDynamic.cpp"
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
enable_reflection(TestReflection ${CMAKE_CURRENT_SOURCE_DIR})

#
# Environment
#

set_target_properties(TestReflection PROPERTIES FOLDER "Tests")

#
# External
#

target_include_directories(TestReflection PUBLIC "../../External/doctest")
