#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

cmake_minimum_required(VERSION 3.16)

#
# Library
#

project(TestReflection)
add_executable(TestReflection
    "TestReflection.cpp"
)

target_compile_features(TestReflection PUBLIC cxx_std_17)
add_test(Reflection TestReflection)

#
# Dependencies
#

if(NOT TARGET TestHelpers)
    add_subdirectory("../Helpers" "TestHelpers")
endif()

target_link_libraries(TestReflection PRIVATE TestHelpers)

if(NOT TARGET Reflection)
    add_subdirectory("../../Source/Reflection" "Reflection")
endif()

target_link_libraries(TestReflection PRIVATE Reflection)

#
# Environment
#

set_target_properties(TestReflection PROPERTIES FOLDER "Tests")
