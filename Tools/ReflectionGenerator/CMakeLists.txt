#
# Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

cmake_minimum_required(VERSION 3.21)
include("../../Build/CMakeShared.cmake")
include_guard(GLOBAL)

#
# Project
#

project(ReflectionGenerator)
custom_cmake_setup()

#
# Executable
#

set(REFLECTION_BINDING_TEMPLATE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ReflectionGenerated.hpp.in")
set_property(GLOBAL PROPERTY REFLECTION_BINDING_TEMPLATE ${REFLECTION_BINDING_TEMPLATE_FILE})

set(SOURCE_FILES
    "ReflectionGenerator.cpp"
    "ReflectionGeneratorTypes.hpp"
    "ReflectionGeneratorUtility.hpp"
    "ReflectionGeneratorUtility.cpp"
    "ReflectionGeneratorCache.hpp"
    "ReflectionGeneratorCache.cpp"
    "ReflectionGeneratorParser.hpp"
    "ReflectionGeneratorParser.cpp"
    "ReflectionGeneratorWriter.hpp"
    "ReflectionGeneratorWriter.cpp"
    ${REFLECTION_BINDING_TEMPLATE_FILE}
)

if(NOT EMSCRIPTEN)
    add_executable(ReflectionGenerator ${SOURCE_FILES})
    export(TARGETS ReflectionGenerator FILE "${CMAKE_BINARY_DIR}/ReflectionGenerator.cmake")

    target_sources(ReflectionGenerator PRIVATE "Precompiled.hpp")
    target_precompile_headers(ReflectionGenerator PRIVATE "Precompiled.hpp")
    source_group("Generated\\Precompiled" REGULAR_EXPRESSION "cmake_pch")
    source_group("" FILES "Precompiled.hpp")

    set_property(TARGET ReflectionGenerator PROPERTY FOLDER "Tools")
    set_property(GLOBAL PROPERTY REFLECTION_GENERATOR ReflectionGenerator)

    source_group("" FILES ${SOURCE_FILES})
else()
    # Import already compiled reflection generator binary.
    include("${CMAKE_BINARY_DIR}/ReflectionGenerator/ReflectionGenerator.cmake")
    get_target_property(REFLECTION_GENERATOR ReflectionGenerator IMPORTED_LOCATION_RELEASE)
    set_property(GLOBAL PROPERTY REFLECTION_GENERATOR ${REFLECTION_GENERATOR})
endif()
