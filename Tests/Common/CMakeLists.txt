#
# Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

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

add_executable(TestCommon ${TEST_FILES})
target_compile_features(TestCommon PUBLIC cxx_std_17)

#
# Precompiled Header
#

target_sources(TestCommon PRIVATE "Precompiled.hpp")
target_precompile_headers(TestCommon PRIVATE "Precompiled.hpp")
source_group("Generated\\Precompiled" REGULAR_EXPRESSION "cmake_pch")
source_group("" FILES "Precompiled.hpp")

#
# Dependencies
#

add_subdirectory("../../Source/Common" "Engine/Common")
target_link_libraries(TestCommon PRIVATE Common)

enable_reflection(TestCommon ${CMAKE_CURRENT_SOURCE_DIR})

#
# Environment
#

set_target_properties(TestCommon PROPERTIES FOLDER "Tests")
set_target_properties(TestCommon PROPERTIES COMPILE_DEFINITIONS
    TESTS_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}")

if(NOT EMSCRIPTEN)
    gtest_discover_tests(TestCommon TEST_PREFIX "Common.")
endif()

#
# External
#

target_link_libraries(TestCommon PRIVATE gtest_main)
