#
# Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Library
#

project(TestEvent)
add_executable(TestEvent "TestEvent.cpp")
target_compile_features(TestEvent PUBLIC cxx_std_17)
add_test("Event" TestEvent)

#
# Dependencies
#

add_subdirectory("../Helpers" "TestHelpers")
target_link_libraries(TestEvent PRIVATE TestHelpers)

add_subdirectory("../../Source/Event" "Event")
target_link_libraries(TestEvent PRIVATE Event)

#
# Environment
#

set_target_properties(TestEvent PROPERTIES FOLDER "Tests")
