#
# Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Files
#

set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/Include/Core")
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

set(FILES_CONFIG
    "${INCLUDE_DIR}/ConfigTypes.hpp"
    "${INCLUDE_DIR}/ConfigSystem.hpp"
    "${SOURCE_DIR}/ConfigSystem.cpp"
)

set(FILES_SYSTEMS
    "${INCLUDE_DIR}/SystemInterface.hpp"
    "${INCLUDE_DIR}/SystemStorage.hpp"
    "${INCLUDE_DIR}/EngineSystem.hpp"
)

set(FILES_METRICS
    "${INCLUDE_DIR}/EngineMetrics.hpp"
    "${SOURCE_DIR}/EngineMetrics.cpp"
)

set(FILES_CORE
    "${INCLUDE_DIR}/Core.hpp"
    "${SOURCE_DIR}/Core.cpp"
)

source_group("Config" FILES ${FILES_CONFIG})
source_group("Systems" FILES ${FILES_SYSTEMS})
source_group("Metrics" FILES ${FILES_METRICS})
source_group("" FILES ${FILES_CORE})

#
# Library
#

add_library(Core
    ${FILES_CONFIG}
    ${FILES_SYSTEMS}
    ${FILES_METRICS}
    ${FILES_CORE}
)

set_property(TARGET Core PROPERTY FOLDER "Engine")
target_compile_features(Core PUBLIC cxx_std_17)
target_include_directories(Core PUBLIC
    "${PROJECT_SOURCE_DIR}/Include"
    "${PROJECT_SOURCE_DIR}/Source"
)

target_sources(Core PRIVATE "${SOURCE_DIR}/Precompiled.hpp")
target_precompile_headers(Core PRIVATE "${SOURCE_DIR}/Precompiled.hpp")
source_group("Generated\\Precompiled" REGULAR_EXPRESSION "cmake_pch")
source_group("" FILES "${SOURCE_DIR}/Precompiled.hpp")

#
# Dependencies
#

add_subdirectory("../Common" "Common")
target_link_libraries(Core PUBLIC Common)

enable_reflection(Core ${INCLUDE_DIR} ${SOURCE_DIR})

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
