#
# Copyright (c) 2018 Piotr Doan. All rights reserved.
#

# CMake version that was used to write this configuration file.
# This is important to set in order to establish policies for many commands.
cmake_minimum_required(VERSION 3.11)

#
# Project
#

# Define our project.
project("GameEngine")

#
# Sources
#

# List of source files.
set(SOURCE_FILES
    "Include/Engine.hpp"
    "Source/Precompiled.hpp"
    "Source/Precompiled.cpp"

    "Include/Common/Build.hpp"
    "Source/Common/Build.cpp"
    "Include/Common/Debug.hpp"
    "Source/Common/Debug.cpp"
    "Include/Common/NonCopyable.hpp"
    "Include/Common/ScopeGuard.hpp"
    "Include/Common/Utility.hpp"
    "Source/Common/Utility.cpp"

    "Include/Events/Delegate.hpp"
    "Include/Events/Collector.hpp"
    "Include/Events/Dispatcher.hpp"
    "Include/Events/Receiver.hpp"

    "Include/Logger/Logger.hpp"
    "Source/Logger/Logger.cpp"
    "Include/Logger/Message.hpp"
    "Source/Logger/Message.cpp"
    "Include/Logger/Sink.hpp"
    "Source/Logger/Sink.cpp"
    "Include/Logger/Format.hpp"
    "Source/Logger/Format.cpp"
    "Include/Logger/Output.hpp"
    "Source/Logger/Output.cpp"

    "Include/System/Platform.hpp"
    "Source/System/Platform.cpp"
    "Include/System/Window.hpp"
    "Source/System/Window.cpp"
    "Include/System/Timer.hpp"
    "Source/System/Timer.cpp"

    "Include/Graphics/RenderContext.hpp"
    "Source/Graphics/RenderContext.cpp"
    "Include/Graphics/RenderState.hpp"
    "Source/Graphics/RenderState.cpp"
    "Include/Graphics/ScreenSpace.hpp"
    "Source/Graphics/ScreenSpace.cpp"
    "Include/Graphics/Buffer.hpp"
    "Source/Graphics/Buffer.cpp"
    "Include/Graphics/VertexArray.hpp"
    "Source/Graphics/VertexArray.cpp"
    "Include/Graphics/Texture.hpp"
    "Source/Graphics/Texture.cpp"
    "Include/Graphics/Sampler.hpp"
    "Source/Graphics/Sampler.cpp"
    "Include/Graphics/Shader.hpp"
    "Source/Graphics/Shader.cpp"
    "Include/Graphics/Sprite.hpp"
    "Source/Graphics/Sprite.cpp"
    "Include/Graphics/SpriteList.hpp"
    "Source/Graphics/SpriteList.cpp"
    "Include/Graphics/SpriteRenderer.hpp"
    "Source/Graphics/SpriteRenderer.cpp"

    "Include/Editor/Editor.hpp"
    "Source/Editor/Editor.cpp"

    "Include/Game/EntityHandle.hpp"
    "Include/Game/EntitySystem.hpp"
    "Source/Game/EntitySystem.cpp"
    "Include/Game/Component.hpp"
    "Include/Game/ComponentPool.hpp"
    "Include/Game/ComponentSystem.hpp"
    "Source/Game/ComponentSystem.cpp"
    "Include/Game/IdentitySystem.hpp"
    "Source/Game/IdentitySystem.cpp"
)

#
# Target
#

# Declare a statically linked library target.
add_library("Engine" STATIC ${SOURCE_FILES})

# Declare compiler features.
target_compile_features("Engine" PUBLIC cxx_std_17)

# Declare include directories.
target_include_directories("Engine" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Include")
target_include_directories("Engine" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Source")
target_include_directories("Engine" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/Source")

#
# Config
#

# Run Git commands to retrieve and save information about current build version.
find_package("Git")

add_custom_target("Configure"
    COMMAND ${CMAKE_COMMAND}
        -D GIT_EXECUTABLE="${GIT_EXECUTABLE}"
        -D ENGINE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
        -D PROJECT_DIR="${CMAKE_SOURCE_DIR}"
        -D INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/Source/Config/BuildInfo.hpp.in"
        -D OUTPUT_FILE="${CMAKE_CURRENT_BINARY_DIR}/Source/Config/BuildInfo.hpp"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/Source/Config/BuildInfo.cmake"
)

add_dependencies("Engine" "Configure")

# Add file with generated build version to engine sources.
# Only the output header file is needed for the build process.
# We have to create a dummy file as it will not be generated yet.
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/Source/Config/BuildInfo.hpp")

target_sources("Engine" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Source/Config/BuildInfo.hpp.in")
source_group("Source\\Config" FILES "${CMAKE_CURRENT_SOURCE_DIR}/Source/Config/BuildInfo.hpp.in")

target_sources("Engine" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Source/Config/BuildInfo.cmake")
source_group("Source\\Config" FILES "${CMAKE_CURRENT_SOURCE_DIR}/Source/Config/BuildInfo.cmake")

target_sources("Engine" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/Source/Config/BuildInfo.hpp")
source_group("Source\\Config" FILES "${CMAKE_CURRENT_BINARY_DIR}/Source/Config/BuildInfo.hpp")

# Write a file pointing at the engine directory.
get_filename_component(ABSOLUTE_ENGINE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Deploy" ABSOLUTE)
file(WRITE "${CMAKE_BINARY_DIR}/EngineDir.txt" "${ABSOLUTE_ENGINE_DIR}/")

#
# Tests
#

# Enable testing.
enable_testing()

# Include tests in the build process.
add_subdirectory("Tests")

#
# Platform
#

# Microsoft Visual Studio
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # Disable Standard C++ Library warnings.
    set_property(TARGET "Engine" APPEND_STRING PROPERTY COMPILE_DEFINITIONS "_CRT_SECURE_NO_WARNINGS")
    set_property(TARGET "Engine" APPEND_STRING PROPERTY COMPILE_DEFINITIONS "_SCL_SECURE_NO_WARNINGS")
    
    # Use precompiled header for quicker compilation of static headers.
    set_source_files_properties(${SOURCE_FILES} PROPERTIES
        COMPILE_FLAGS "/Yu\"Precompiled.hpp\" /Fp\"${CMAKE_CURRENT_BINARY_DIR}/Source/Precompiled-$(Configuration).pch\""
    )

    set_source_files_properties("Source/Precompiled.cpp" PROPERTIES
        COMPILE_FLAGS "/Yc\"Precompiled.hpp\" /Fp\"${CMAKE_CURRENT_BINARY_DIR}/Source/Precompiled-$(Configuration).pch\""
    )
endif()

# Keep folder structure of include and source files in Visual Studio.
source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCE_FILES})

# Enable source folders in Visual Studio.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

#
# External
#

# Add GLM library.
target_include_directories("Engine" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/External/GLM-0.9.9.0")

# Add OpenGL library.
find_package(OpenGL REQUIRED)
target_link_libraries("Engine" ${OPENGL_GL_LIBRARY})

# Add GLEW library.
set(BUILD_UTILS OFF CACHE BOOL "")

add_subdirectory("External/GLEW-2.1.0/build/cmake" "External/GLEW" EXCLUDE_FROM_ALL)
target_include_directories("Engine" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/External/GLEW-2.1.0/include")
target_link_libraries("Engine" "glew_s")

set_property(TARGET "glew_s" PROPERTY PROJECT_LABEL "GLEW")
set_property(TARGET "glew_s" PROPERTY FOLDER "External")

# Add GLFW library.
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")

add_subdirectory("External/GLFW-3.2.1" "External/GLFW" EXCLUDE_FROM_ALL)
target_include_directories("Engine" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/External/GLFW-3.2.1/include")
target_link_libraries("Engine" "glfw")

set_property(TARGET "glfw" PROPERTY PROJECT_LABEL "GLFW")
set_property(TARGET "glfw" PROPERTY FOLDER "External")

# Add ImGui library.
add_subdirectory("External/ImGui-1.62" "External/ImGui" EXCLUDE_FROM_ALL)
target_include_directories("ImGui" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/External/GLEW-2.1.0/include")
target_link_libraries("Engine" "ImGui")

set_property(TARGET "ImGui" PROPERTY FOLDER "External")

# Add zlib library.
set(SKIP_INSTALL_ALL ON CACHE BOOL "")

add_subdirectory("External/zlib-1.2.11" "External/zlib" EXCLUDE_FROM_ALL)
target_include_directories("Engine" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/External/zlib-1.2.11")
target_include_directories("Engine" PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/External/zlib")
target_link_libraries("Engine" "zlibstatic")

set_property(TARGET "zlibstatic" PROPERTY PROJECT_LABEL "zlib")
set_property(TARGET "zlibstatic" PROPERTY FOLDER "External")

set(ZLIB_ROOT "External/zlib-1.2.11")
set(ZLIB_INCLUDE_DIR "External/zlib-1.2.11")
set(ZLIB_LIBRARY "zlibstatic")

# Add libpng library.
set(PNG_SHARED OFF CACHE BOOL "")
set(PNG_STATIC ON CACHE BOOL "")
set(PNG_TESTS OFF CACHE BOOL "")
set(SKIP_INSTALL_ALL ON CACHE BOOL "")

add_subdirectory("External/libpng-1.6.34" "External/libpng" EXCLUDE_FROM_ALL)
target_include_directories("png_static" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/External/zlib-1.2.11")
target_include_directories("png_static" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/External/zlib")
target_include_directories("Engine" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/External/libpng-1.6.34")
target_include_directories("Engine" PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/External/libpng")
target_link_libraries("Engine" "png_static")

set_property(TARGET "png_static" PROPERTY PROJECT_LABEL "libpng")
set_property(TARGET "png_static" PROPERTY FOLDER "External")

set_property(TARGET "genfiles" PROPERTY PROJECT_LABEL "libpngconf")
set_property(TARGET "genfiles" PROPERTY FOLDER "External")
