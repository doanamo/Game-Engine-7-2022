#
# Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Library
#

project(TestResult)
add_executable(TestResult "TestResult.cpp")
target_compile_features(TestResult PUBLIC cxx_std_17)
add_test("Result" TestResult)

#
# Dependencies
#

add_subdirectory("../Helpers" "TestHelpers")
target_link_libraries(TestResult PRIVATE TestHelpers)

add_subdirectory("../../Source/Common" "Common")
target_link_libraries(TestResult PRIVATE Common)

#
# Environment
#

set_target_properties(TestResult PROPERTIES FOLDER "Tests")
