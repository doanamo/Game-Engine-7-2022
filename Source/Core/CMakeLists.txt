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
    "Service.hpp"
    "ServiceStorage.hpp"
    "PerformanceMetrics.hpp"
)

set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../Include/Core/")
list(TRANSFORM INCLUDE_FILES PREPEND ${INCLUDE_DIR})
source_group(TREE ${INCLUDE_DIR} PREFIX "Header Files" FILES ${INCLUDE_FILES})

set(SOURCE_FILES
    "Precompiled.hpp"
    "Core.cpp"
    "ServiceStorage.cpp"
    "PerformanceMetrics.cpp"
)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/")
list(TRANSFORM SOURCE_FILES PREPEND ${SOURCE_DIR})
source_group(TREE ${SOURCE_DIR} PREFIX "Source Files" FILES ${SOURCE_FILES})

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
target_link_libraries(Core PRIVATE Reflection)
enable_reflection(Core ${SOURCE_DIR} ${INCLUDE_DIR})

#
# Externals
#

target_include_directories(Core PUBLIC "../../External/utfcpp/source")
target_include_directories(Core PUBLIC "../../External/glm")

if(NOT EMSCRIPTEN)
    add_subdirectory("../../External/zlib" "External/zlib" EXCLUDE_FROM_ALL)
    target_include_directories(Core PUBLIC "../../External/zlib")
    target_include_directories(Core PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/External/zlib")
    target_link_libraries(Core PUBLIC "zlibstatic")
    set_property(TARGET "zlibstatic" PROPERTY PROJECT_LABEL "zlib")
    set_property(TARGET "zlibstatic" PROPERTY FOLDER "External")

    set(PNG_BUILD_ZLIB ON)
    add_subdirectory("../../External/libpng" "External/libpng")
    target_include_directories("png_static" PRIVATE "../../External/zlib")
    target_include_directories("png_static" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/External/zlib")
    target_include_directories(Core PUBLIC "../../External/libpng")
    target_include_directories(Core PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/External/libpng")
    target_link_libraries(Core PUBLIC "png_static")
    set_property(TARGET "png_static" PROPERTY PROJECT_LABEL "libpng")
    set_property(TARGET "png_static" PROPERTY FOLDER "External")
    set_property(TARGET "genfiles" PROPERTY PROJECT_LABEL "libpngconf")
    set_property(TARGET "genfiles" PROPERTY FOLDER "External")

    find_package(OpenGL REQUIRED)
    target_link_libraries(Core ${OPENGL_GL_LIBRARY})

    add_subdirectory("../../External/glfw" "External/glfw")
    target_include_directories(Core PUBLIC "../../External/glfw/include")
    target_link_libraries(Core PUBLIC "glfw")
    set_property(TARGET "glfw" PROPERTY FOLDER "External")
else()
    target_compile_options(Core PUBLIC "SHELL: -s USE_ZLIB=1")
    target_link_options(Core PUBLIC "SHELL: -s USE_ZLIB=1")

    target_compile_options(Core PUBLIC "SHELL: -s USE_LIBPNG=1")
    target_link_options(Core PUBLIC "SHELL: -s USE_LIBPNG=1")

    target_compile_options(Core PUBLIC "SHELL: -s USE_GLFW=3")
    target_link_options(Core PUBLIC "SHELL: -s USE_GLFW=3")

    target_compile_options(Core PUBLIC "SHELL: -s FULL_ES3=1")
    target_link_options(Core PUBLIC "SHELL: -s FULL_ES3=1")
endif()

add_subdirectory("../../External/glad/" "External/glad")
target_link_libraries(Core PUBLIC "glad")
set_property(TARGET "glad" PROPERTY FOLDER "External")

add_subdirectory("../../External/imgui" "External/imgui")
target_include_directories("imgui" PUBLIC "../../External/imgui/include")
target_link_libraries(Core PUBLIC "imgui")
set_property(TARGET "imgui" PROPERTY FOLDER "External")
