#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Library
#

project(TestScopeGuard)
add_executable(TestScopeGuard "TestScopeGuard.cpp")
target_compile_features(TestScopeGuard PUBLIC cxx_std_17)
add_test("ScopeGuard" TestScopeGuard)

#
# Dependencies
#

add_subdirectory("../Helpers" "TestHelpers")
target_link_libraries(TestScopeGuard PRIVATE TestHelpers)

add_subdirectory("../../Source/Common" "Common")
target_link_libraries(TestScopeGuard PRIVATE Common)

#
# Environment
#

set_target_properties(TestScopeGuard PROPERTIES FOLDER "Tests")
