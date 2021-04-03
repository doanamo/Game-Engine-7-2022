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
    "TestUtility.cpp"
    "TestScopeGuard.cpp"
    "TestResult.cpp"
    "TestStateMachine.cpp"
    "TestHandleMap.cpp"
    "TestEvent.cpp"
    "TestName.cpp"
)

#
# Test
#

project(TestCommon)
add_executable(TestCommon ${TEST_FILES})
target_compile_features(TestCommon PUBLIC cxx_std_17)
add_test("Common" TestCommon)

#
# Dependencies
#

add_subdirectory("../../Source/Reflection" "Reflection")
target_link_libraries(TestCommon PRIVATE Reflection)
enable_reflection(TestCommon ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory("../../Source/Common" "Common")
target_link_libraries(TestCommon PRIVATE Common)

#
# Environment
#

set_target_properties(TestCommon PROPERTIES FOLDER "Tests")
set_target_properties(TestCommon PROPERTIES COMPILE_DEFINITIONS
    TESTS_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}")

#
# External
#

target_include_directories(TestCommon PUBLIC "../../External/doctest")
