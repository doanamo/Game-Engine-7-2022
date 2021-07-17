#
# Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Files
#

set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/Include/Reflection")
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

set(FILES_DETAIL
    "${INCLUDE_DIR}/ReflectionUtility.hpp"
    "${INCLUDE_DIR}/ReflectionDetail.hpp"
    "${INCLUDE_DIR}/ReflectionStatic.hpp"
    "${INCLUDE_DIR}/ReflectionDynamic.hpp"
    "${INCLUDE_DIR}/ReflectionRegistry.hpp"
    "${INCLUDE_DIR}/ReflectionDeclare.hpp"
    "${INCLUDE_DIR}/ReflectionTypes.hpp"
    "${SOURCE_DIR}/ReflectionDynamic.cpp"
    "${SOURCE_DIR}/ReflectionRegistry.cpp"
)

set(FILES_REFLECTION
    "${INCLUDE_DIR}/Reflection.hpp"
    "${SOURCE_DIR}/Reflection.cpp"
)

source_group("Detail" FILES ${FILES_DETAIL})
source_group("" FILES ${FILES_REFLECTION})

#
# Library
#

add_library(Reflection
    ${FILES_DETAIL}
    ${FILES_REFLECTION}
)

set_property(TARGET Reflection PROPERTY FOLDER "Engine")
target_compile_features(Reflection PUBLIC cxx_std_17)
target_include_directories(Reflection PUBLIC
    "${PROJECT_SOURCE_DIR}/Include"
    "${PROJECT_SOURCE_DIR}/Source"
)

target_sources(Reflection PRIVATE "${SOURCE_DIR}/Precompiled.hpp")
target_precompile_headers(Reflection PRIVATE "${SOURCE_DIR}/Precompiled.hpp")
source_group("Generated\\Precompiled" REGULAR_EXPRESSION "cmake_pch")
source_group("" FILES "${SOURCE_DIR}/Precompiled.hpp")

#
# Dependencies
#

add_subdirectory("../Common" "Common")
target_link_libraries(Reflection PUBLIC Common)

#
# Generator
#

add_subdirectory("../../Tools/ReflectionGenerator" "ReflectionGenerator")

function(collect_reflected_dependencies TARGET_NAME OUTPUT_LIST)
    get_target_property(TARGET_LINK_LIBRARIES ${TARGET_NAME} LINK_LIBRARIES)

    if(TARGET_LINK_LIBRARIES)
        foreach(TARGET_LINK_LIBRARY IN LISTS TARGET_LINK_LIBRARIES)
            get_target_property(REFLECTION_ENABLED ${TARGET_LINK_LIBRARY} REFLECTION_ENABLED)

            if(REFLECTION_ENABLED)
                collect_reflected_dependencies(${TARGET_LINK_LIBRARY} ${OUTPUT_LIST})
                list(APPEND ${OUTPUT_LIST} ${TARGET_LINK_LIBRARY})
                list(REMOVE_DUPLICATES ${OUTPUT_LIST})
            endif()
        endforeach()
    endif()

    set(${OUTPUT_LIST} ${${OUTPUT_LIST}} PARENT_SCOPE)
endfunction()

function(enable_reflection TARGET_NAME)
    get_property(REFLECTION_GENERATOR GLOBAL PROPERTY REFLECTION_GENERATOR)
    get_target_property(TARGET_TYPE ${TARGET_NAME} TYPE)

    collect_reflected_dependencies(${TARGET_NAME} TARGET_DEPENDENCIES)

    add_custom_target(Reflection${TARGET_NAME}
        COMMAND ${CMAKE_COMMAND} -E echo "Generating reflection binding for ${TARGET_NAME}..."
        COMMAND ${REFLECTION_GENERATOR}
            ${TARGET_TYPE} ${TARGET_NAME} "\"${TARGET_DEPENDENCIES}\""
            ${CMAKE_CURRENT_BINARY_DIR} ${ARGN}
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

    source_group("Generated\\Reflection" REGULAR_EXPRESSION "ReflectionGenerated")
    set_target_properties(${TARGET_NAME} PROPERTIES REFLECTION_ENABLED TRUE)
endfunction()
