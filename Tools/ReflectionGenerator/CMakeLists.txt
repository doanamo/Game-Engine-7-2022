#
# Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Executable
#

set(REFLECTION_BINDING_TEMPLATE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ReflectionGenerated.hpp.in")
set_property(GLOBAL PROPERTY REFLECTION_BINDING_TEMPLATE ${REFLECTION_BINDING_TEMPLATE_FILE})

set(SOURCE_FILES
    "ReflectionGenerator.cpp"
    ${REFLECTION_BINDING_TEMPLATE_FILE}
)

if(NOT EMSCRIPTEN)
    project(ReflectionGenerator)
    add_executable(ReflectionGenerator ${SOURCE_FILES})
    target_compile_features(ReflectionGenerator PUBLIC cxx_std_17)
    export(TARGETS ReflectionGenerator FILE "${CMAKE_BINARY_DIR}/ReflectionGenerator.cmake")

    set_property(TARGET ReflectionGenerator PROPERTY FOLDER "Tools")
    set_property(GLOBAL PROPERTY REFLECTION_GENERATOR ReflectionGenerator)
else()
    include("${CMAKE_BINARY_DIR}/ReflectionGenerator/ReflectionGenerator.cmake")

    get_target_property(REFLECTION_GENERATOR ReflectionGenerator IMPORTED_LOCATION_RELEASE)
    set_property(GLOBAL PROPERTY REFLECTION_GENERATOR ${REFLECTION_GENERATOR})
endif()
