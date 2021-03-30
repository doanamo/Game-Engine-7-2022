#
# Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Executable
#

project(ReflectionGenerator)
add_executable(ReflectionGenerator "ReflectionGenerator.cpp")
target_compile_features(ReflectionGenerator PUBLIC cxx_std_17)
set_property(TARGET ReflectionGenerator PROPERTY FOLDER "Tools")

set_property(GLOBAL PROPERTY REFLECTION_BINDING_TEMPLATE
    "${CMAKE_CURRENT_SOURCE_DIR}/ReflectionGenerated.hpp.in")
