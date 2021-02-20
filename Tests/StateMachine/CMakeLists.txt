#
# Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Library
#

project(TestStateMachine)
add_executable(TestStateMachine "TestStateMachine.cpp")
target_compile_features(TestStateMachine PUBLIC cxx_std_17)
add_test("StateMachine" TestStateMachine)

#
# Dependencies
#

add_subdirectory("../Helpers" "TestHelpers")
target_link_libraries(TestStateMachine PRIVATE TestHelpers)

add_subdirectory("../../Source/Common" "Common")
target_link_libraries(TestStateMachine PRIVATE Common)

#
# Environment
#

set_target_properties(TestStateMachine PROPERTIES FOLDER "Tests")
