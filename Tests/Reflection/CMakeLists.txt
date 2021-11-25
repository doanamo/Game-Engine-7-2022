#
# Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

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

add_executable(TestReflection ${TEST_FILES})
target_compile_features(TestReflection PUBLIC cxx_std_17)

#
# Dependencies
#

add_subdirectory("../../Source/Reflection" "Engine/Reflection")
target_link_libraries(TestReflection PRIVATE Reflection)

enable_reflection(TestReflection ${CMAKE_CURRENT_SOURCE_DIR})

#
# Environment
#

set_target_properties(TestReflection PROPERTIES FOLDER "Tests")

#
# External
#

target_link_libraries(TestReflection PRIVATE gtest_main)
gtest_discover_tests(TestReflection TEST_PREFIX "Reflection.")
