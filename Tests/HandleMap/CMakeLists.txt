#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Library
#

project(TestHandleMap)
add_executable(TestHandleMap "TestHandleMap.cpp")
target_compile_features(TestHandleMap PUBLIC cxx_std_17)
add_test("HandleMap" TestHandleMap)

#
# Dependencies
#

add_subdirectory("../Helpers" "TestHelpers")
target_link_libraries(TestHandleMap PRIVATE TestHelpers)

add_subdirectory("../../Source/Common" "Common")
target_link_libraries(TestHandleMap PRIVATE Common)

#
# Environment
#

set_target_properties(TestHandleMap PROPERTIES FOLDER "Tests")
