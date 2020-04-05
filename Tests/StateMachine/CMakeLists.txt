#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

cmake_minimum_required(VERSION 3.16)

#
# Library
#

project(TestStateMachine)
add_executable(TestStateMachine
    "TestStateMachine.cpp"
)

target_compile_features(TestStateMachine PUBLIC cxx_std_17)
add_test(StateMachine TestStateMachine)

#
# Dependencies
#

if(NOT TARGET TestHelpers)
    add_subdirectory("../Helpers" "TestHelpers")
endif()

target_link_libraries(TestStateMachine PRIVATE TestHelpers)

if(NOT TARGET Common)
    add_subdirectory("../../Source/Common" "Common")
endif()

target_link_libraries(TestStateMachine PRIVATE Common)

#
# Environment
#

set_target_properties(TestStateMachine PROPERTIES FOLDER "Tests")
