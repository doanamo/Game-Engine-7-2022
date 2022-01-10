#
# Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

include_guard(GLOBAL)

#
# Files
#

set(TEST_FILES
    "TestGame.cpp"
    "TestIdentitySystem.cpp"
)

#
# Test
#

add_executable(TestGame ${TEST_FILES})
target_compile_features(TestGame PUBLIC cxx_std_17)

#
# Precompiled Header
#

target_sources(TestGame PRIVATE "Precompiled.hpp")
target_precompile_headers(TestGame PRIVATE "Precompiled.hpp")
source_group("Generated\\Precompiled" REGULAR_EXPRESSION "cmake_pch")
source_group("" FILES "Precompiled.hpp")

#
# Dependencies
#

add_subdirectory("../../Source/Core" "Engine/Core")
target_link_libraries(TestGame PRIVATE Core)

add_subdirectory("../../Source/Game" "Engine/Game")
target_link_libraries(TestGame PRIVATE Game)

enable_reflection(TestGame ${CMAKE_CURRENT_SOURCE_DIR})

#
# Environment
#

set_target_properties(TestGame PROPERTIES FOLDER "Tests")

if(NOT EMSCRIPTEN)
    gtest_discover_tests(TestGame TEST_PREFIX "Game.")
endif()

#
# External
#

target_link_libraries(TestGame PRIVATE gtest_main)
