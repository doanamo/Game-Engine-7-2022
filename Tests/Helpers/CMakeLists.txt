#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Library
#

project(TestHelpers)
add_library(TestHelpers STATIC "TestHelpers.hpp" "Empty.cpp")
target_compile_features(TestHelpers PUBLIC cxx_std_17)
target_include_directories(TestHelpers PUBLIC ".")

#
# Dependencies
#

add_subdirectory("../../Source/Common" "Common")
target_link_libraries(TestHelpers PUBLIC Common)

#
# Environment
#

set_target_properties(TestHelpers PROPERTIES FOLDER "Tests")
