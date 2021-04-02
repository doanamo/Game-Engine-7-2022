#
# Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Files
#

set(INCLUDE_FILES
    "Reflection.hpp"
    "ReflectionUtility.hpp"
    "ReflectionDetail.hpp"
    "ReflectionStatic.hpp"
    "ReflectionDynamic.hpp"
    "ReflectionRegistry.hpp"
    "ReflectionDeclare.hpp"
    "ReflectionTypes.hpp"
)

set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../Include/Reflection/")
list(TRANSFORM INCLUDE_FILES PREPEND ${INCLUDE_DIR})
source_group(TREE ${INCLUDE_DIR} PREFIX "Header Files" FILES ${INCLUDE_FILES})

set(SOURCE_FILES
    "Precompiled.hpp"
    "ReflectionDynamic.cpp"
    "ReflectionRegistry.cpp"
    "ReflectionTypes.cpp"
)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/")
list(TRANSFORM SOURCE_FILES PREPEND ${SOURCE_DIR})
source_group(TREE ${SOURCE_DIR} PREFIX "Source Files" FILES ${SOURCE_FILES})

#
# Library
#

project(Reflection)
add_library(Reflection ${INCLUDE_FILES} ${SOURCE_FILES})
target_compile_features(Reflection PUBLIC cxx_std_17)
target_include_directories(Reflection PUBLIC "../../Include" "../../Source")
target_precompile_headers(Reflection PRIVATE "Precompiled.hpp")
set_property(TARGET Reflection PROPERTY FOLDER "Engine")

#
# Dependencies
#

add_subdirectory("../Common" "Common")
target_link_libraries(Reflection PUBLIC Common)

#
# Generator
#

add_subdirectory("../../Tools/ReflectionGenerator" "ReflectionGenerator")

function(enable_reflection TARGET_NAME)
    get_property(REFLECTION_GENERATOR GLOBAL PROPERTY REFLECTION_GENERATOR)

    add_custom_target(Reflection${TARGET_NAME}
        COMMAND ${CMAKE_COMMAND} -E echo "Generating reflection binding for ${TARGET_NAME}..."
        COMMAND ${REFLECTION_GENERATOR} ${TARGET_NAME} ${CMAKE_CURRENT_BINARY_DIR} ${ARGN}
    )

    add_dependencies(${TARGET_NAME} Reflection${TARGET_NAME})
    set_property(TARGET Reflection${TARGET_NAME} PROPERTY FOLDER "Generated")
    target_include_directories(${TARGET_NAME} PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/Include/")

    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/ReflectionGenerated.cpp")
    target_sources(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/ReflectionGenerated.cpp")

    get_property(REFLECTION_BINDING_TEMPLATE GLOBAL PROPERTY REFLECTION_BINDING_TEMPLATE)
    configure_file(${REFLECTION_BINDING_TEMPLATE}
        "${CMAKE_CURRENT_BINARY_DIR}/Include/${TARGET_NAME}/ReflectionGenerated.hpp")
    target_sources(${TARGET_NAME} PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/Include/${TARGET_NAME}/ReflectionGenerated.hpp")
endfunction()
