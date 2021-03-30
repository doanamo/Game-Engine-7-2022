#
# Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Executable
#

if(NOT EMSCRIPTEN)
    project(ReflectionGenerator)
    add_executable(ReflectionGenerator "ReflectionGenerator.cpp")
    target_compile_features(ReflectionGenerator PUBLIC cxx_std_17)
    set_property(TARGET ReflectionGenerator PROPERTY FOLDER "Tools")
    export(TARGETS ReflectionGenerator FILE "${CMAKE_BINARY_DIR}/ReflectionGenerator.cmake")
else()
    include("${CMAKE_BINARY_DIR}/ReflectionGenerator/ReflectionGenerator.cmake")
endif()

if(NOT EMSCRIPTEN)
    set_property(GLOBAL PROPERTY REFLECTION_GENERATOR ReflectionGenerator)
else()
    message(${CMAKE_BUILD_TYPE})
    get_target_property(REFLECTION_GENERATOR ReflectionGenerator IMPORTED_LOCATION_RELEASE)
    set_property(GLOBAL PROPERTY REFLECTION_GENERATOR ${REFLECTION_GENERATOR})
endif()

set_property(GLOBAL PROPERTY REFLECTION_BINDING_TEMPLATE
    "${CMAKE_CURRENT_SOURCE_DIR}/ReflectionGenerated.hpp.in")
