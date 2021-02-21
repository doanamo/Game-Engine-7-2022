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
    "TestCommon.cpp"
    "TestScopeGuard.cpp"
    "TestHandleMap.cpp"
    "TestEvent.cpp"
)

#
# Library
#

project(TestCommon)
add_executable(TestCommon ${TEST_FILES})
target_compile_features(TestCommon PUBLIC cxx_std_17)
add_test("Common" TestCommon)

#
# Dependencies
#

add_subdirectory("../../Source/Common" "Common")
target_link_libraries(TestCommon PRIVATE Common)

#
# Environment
#

set_target_properties(TestCommon PROPERTIES FOLDER "Tests")

#
# External
#

target_include_directories(TestCommon PUBLIC "../../External/doctest")
