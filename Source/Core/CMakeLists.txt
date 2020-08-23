#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
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
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/../../Include/Core/" PREFIX "Header Files" FILES ${INCLUDE_FILES})

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
target_include_directories(Core PUBLIC "../../Include")
target_precompile_headers(Core PRIVATE "Precompiled.hpp")
set_property(TARGET Core PROPERTY FOLDER "Engine")

#
# Dependencies
#

add_subdirectory("../Common" "Common")
target_link_libraries(Core PUBLIC Common)

add_subdirectory("../Logger" "Logger")
target_link_libraries(Core PUBLIC Logger)

add_subdirectory("../Event" "Event")
target_link_libraries(Core PUBLIC Event)

add_subdirectory("../Reflection" "Reflection")
target_link_libraries(Core PUBLIC Reflection)

#
# Externals
#

target_include_directories(Core PUBLIC "../../External/utfcpp-2.3.6/source")
target_include_directories(Core PUBLIC "../../External/GLM-0.9.9.3")

if(NOT EMSCRIPTEN)
    add_subdirectory("../../External/zlib-1.2.11" "External/zlib" EXCLUDE_FROM_ALL)
    target_include_directories(Core PUBLIC "../../External/zlib-1.2.11")
    target_include_directories(Core PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/External/zlib")
    target_link_libraries(Core PUBLIC "zlibstatic")
    set_property(TARGET "zlibstatic" PROPERTY PROJECT_LABEL "zlib")
    set_property(TARGET "zlibstatic" PROPERTY FOLDER "External")

    set(PNG_BUILD_ZLIB ON)
    add_subdirectory("../../External/libpng-1.6.34" "External/libpng")
    target_include_directories("png_static" PRIVATE "../../External/zlib-1.2.11")
    target_include_directories("png_static" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/External/zlib")
    target_include_directories(Core PUBLIC "../../External/libpng-1.6.34")
    target_include_directories(Core PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/External/libpng")
    target_link_libraries(Core PUBLIC "png_static")
    set_property(TARGET "png_static" PROPERTY PROJECT_LABEL "libpng")
    set_property(TARGET "png_static" PROPERTY FOLDER "External")
    set_property(TARGET "genfiles" PROPERTY PROJECT_LABEL "libpngconf")
    set_property(TARGET "genfiles" PROPERTY FOLDER "External")

    find_package(OpenGL REQUIRED)
    target_link_libraries(Core ${OPENGL_GL_LIBRARY})

    add_subdirectory("../../External/GLFW-3.2.1" "External/GLFW")
    target_include_directories(Core PUBLIC "../../External/GLFW-3.2.1/include")
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

add_subdirectory("../../External/glad-0.1.33/" "External/glad")
target_link_libraries(Core PUBLIC "glad")
set_property(TARGET "glad" PROPERTY FOLDER "External")

add_subdirectory("../../External/ImGui-1.62" "External/ImGui")
target_include_directories("ImGui" PUBLIC "../../External/ImGui-1.62/include")
target_link_libraries(Core PUBLIC "ImGui")
set_property(TARGET "ImGui" PROPERTY PROJECT_LABEL "imgui")
set_property(TARGET "ImGui" PROPERTY FOLDER "External")
