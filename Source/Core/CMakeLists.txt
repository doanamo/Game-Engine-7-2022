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
    "Core.hpp"
    "ServiceStorage.hpp"
    "PerformanceMetrics.hpp"
)

list(TRANSFORM INCLUDE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/../../Include/Core/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/../../Include/Core/"
    PREFIX "Header Files" FILES ${INCLUDE_FILES})

set(SOURCE_FILES
    "Precompiled.hpp"
    "Core.cpp"
    "ServiceStorage.cpp"
    "PerformanceMetrics.cpp"
)

list(TRANSFORM SOURCE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/" PREFIX "Source Files" FILES ${SOURCE_FILES})

#
# Library
#

project(Core)
add_library(Core ${INCLUDE_FILES} ${SOURCE_FILES})
target_compile_features(Core PUBLIC cxx_std_17)
target_include_directories(Core PUBLIC "../../Include" "../../Source")
target_precompile_headers(Core PRIVATE "Precompiled.hpp")
set_property(TARGET Core PROPERTY FOLDER "Engine")

#
# Dependencies
#

add_subdirectory("../Common" "Common")
target_link_libraries(Core PUBLIC Common)

add_subdirectory("../Reflection" "Reflection")
target_link_libraries(Core PUBLIC Reflection)

#
# Externals
#

target_include_directories(Core PUBLIC "../../External/utfcpp/source")
target_include_directories(Core PUBLIC "../../External/glm")

add_subdirectory("../../External/imgui" "External/imgui")
target_include_directories("imgui" PUBLIC "../../External/imgui/include")
target_link_libraries(Core PUBLIC "imgui")
set_property(TARGET "imgui" PROPERTY FOLDER "External")
