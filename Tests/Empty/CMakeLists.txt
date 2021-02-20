#
# Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Library
#

project(TestEmpty)
add_executable(TestEmpty "TestEmpty.cpp")
target_compile_features(TestEmpty PUBLIC cxx_std_17)
add_test("Empty" TestEmpty)

#
# Dependencies
#

add_subdirectory("../Helpers" "TestHelpers")
target_link_libraries(TestEmpty PRIVATE TestHelpers)

add_subdirectory("../../Source/Common" "Common")
target_link_libraries(TestEmpty PRIVATE Common)

#
# Environment
#

set_target_properties(TestEmpty PROPERTIES FOLDER "Tests")
