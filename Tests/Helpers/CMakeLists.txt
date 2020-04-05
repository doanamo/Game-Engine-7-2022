#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

cmake_minimum_required(VERSION 3.16)

#
# Library
#

project(TestHelpers)
add_library(TestHelpers STATIC
    "TestHelpers.cpp"
)

target_compile_features(TestHelpers PUBLIC cxx_std_17)
target_include_directories(TestHelpers PUBLIC ".")

#
# Dependencies
#

if(NOT TARGET Common)
    add_subdirectory("../../Source/Common" "Common")
endif()

target_link_libraries(TestHelpers PUBLIC Common)
