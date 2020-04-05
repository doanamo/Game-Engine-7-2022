#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

cmake_minimum_required(VERSION 3.16)

#
# Library
#

project(TestScopeGuard)
add_executable(TestScopeGuard
    "TestScopeGuard.cpp"
)

target_compile_features(TestScopeGuard PUBLIC cxx_std_17)
add_test(ScopeGuard TestScopeGuard)

#
# Dependencies
#

if(NOT TARGET TestHelpers)
    add_subdirectory("../Helpers" "TestHelpers")
endif()

target_link_libraries(TestScopeGuard PRIVATE TestHelpers)

if(NOT TARGET Common)
    add_subdirectory("../../Source/Common" "Common")
endif()

target_link_libraries(TestScopeGuard PRIVATE Common)

#
# Environment
#

set_target_properties(TestScopeGuard PROPERTIES FOLDER "Tests")
