#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

cmake_minimum_required(VERSION 3.16)

#
# Library
#

project(TestUtility)
add_executable(TestUtility
    "TestUtility.cpp"
)

target_compile_features(TestUtility PUBLIC cxx_std_17)
add_test(Utility TestUtility)

#
# Dependencies
#

if(NOT TARGET TestHelpers)
    add_subdirectory("../Helpers" "TestHelpers")
endif()

target_link_libraries(TestUtility PRIVATE TestHelpers)

if(NOT TARGET Common)
    add_subdirectory("../../Source/Common" "Common")
endif()

target_link_libraries(TestUtility PRIVATE Common)

#
# Environment
#

set_target_properties(TestUtility PROPERTIES FOLDER "Tests")
set_target_properties(TestUtility PROPERTIES COMPILE_DEFINITIONS TESTS_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}")
