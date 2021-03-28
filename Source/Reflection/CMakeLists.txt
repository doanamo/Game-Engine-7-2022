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

list(TRANSFORM INCLUDE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/../../Include/Reflection/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/../../Include/Reflection/"
    PREFIX "Header Files" FILES ${INCLUDE_FILES})

set(SOURCE_FILES
    "Precompiled.hpp"
    "ReflectionDynamic.cpp"
    "ReflectionRegistry.cpp"
    "ReflectionTypes.cpp"
)

list(TRANSFORM SOURCE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/" PREFIX "Source Files" FILES ${SOURCE_FILES})

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
# Binding
#

add_executable(ReflectionBinding "ReflectionBinding.cpp")
target_compile_features(ReflectionBinding PUBLIC cxx_std_17)
set_property(TARGET ReflectionBinding PROPERTY FOLDER "Generated")

function(enable_reflection TARGET_NAME)
    add_custom_command(
        TARGET ${TARGET_NAME} PRE_BUILD DEPENDS ReflectionBinding
        COMMAND ${CMAKE_COMMAND} -E echo "Generating reflection binding for ${TARGET_NAME}..."
        COMMAND ReflectionBinding ${TARGET_NAME} ${CMAKE_CURRENT_BINARY_DIR} ${ARGN}
        BYPRODUCTS "ReflectionGenerated.cpp"
    )
    target_sources(${TARGET_NAME} PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/ReflectionGenerated.cpp")
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/ReflectionGenerated.cpp")
endfunction()
