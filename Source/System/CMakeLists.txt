#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

cmake_minimum_required(VERSION 3.16)

#
# Library
#

add_library(System STATIC
    "../../Include/System/Platform.hpp"
    "../../Include/System/FileSystem.hpp"
    "../../Include/System/Window.hpp"
    "../../Include/System/Timer.hpp"
    "../../Include/System/InputDefinitions.hpp"
    "../../Include/System/InputManager.hpp"
    "../../Include/System/ResourcePool.hpp"
    "../../Include/System/ResourceManager.hpp"
    "Precompiled.hpp"
    "Platform.cpp"
    "FileSystem.cpp"
    "Window.cpp"
    "Timer.cpp"
    "InputDefinitions.cpp"
    "InputManager.cpp"
    "ResourceManager.cpp"
)

target_compile_features(System PUBLIC cxx_std_17)
target_include_directories(System PUBLIC "../../Include")
target_precompile_headers(System PRIVATE "Precompiled.hpp")

#
# Dependencies
#

add_subdirectory("../Common" "Common" EXCLUDE_FROM_ALL)
target_link_libraries(System PRIVATE Common)

#
# Externals
#

find_package(OpenGL REQUIRED)
target_link_libraries(System ${OPENGL_GL_LIBRARY})

add_subdirectory("../../External/GLEW-2.1.0/build/cmake" "External/GLEW" EXCLUDE_FROM_ALL)
target_include_directories(System PUBLIC "../../External/GLEW-2.1.0/include")
target_link_libraries(System PUBLIC "glew_s")
set_property(TARGET "glew_s" PROPERTY PROJECT_LABEL "GLEW")
set_property(TARGET "glew_s" PROPERTY FOLDER "External")

add_subdirectory("../../External/GLFW-3.2.1" "External/GLFW" EXCLUDE_FROM_ALL)
target_include_directories(System PUBLIC "../../External/GLFW-3.2.1/include")
target_link_libraries(System PUBLIC "glfw")
set_property(TARGET "glfw" PROPERTY PROJECT_LABEL "GLFW")
set_property(TARGET "glfw" PROPERTY FOLDER "External")
