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
    "RenderContext.hpp"
    "RenderState.hpp"
    "ScreenSpace.hpp"
    "Buffer.hpp"
    "VertexArray.hpp"
    "Texture.hpp"
    "TextureView.hpp"
    "TextureAtlas.hpp"
    "Sampler.hpp"
    "Shader.hpp"
    "Sprite/Sprite.hpp"
    "Sprite/SpriteDrawList.hpp"
    "Sprite/SpriteRenderer.hpp"
    "Sprite/SpriteAnimationList.hpp"
)

list(TRANSFORM INCLUDE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/../../Include/Graphics/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/../../Include/Graphics/" PREFIX "Header Files" FILES ${INCLUDE_FILES})

set(SOURCE_FILES
    "Precompiled.hpp"
    "RenderContext.cpp"
    "RenderState.cpp"
    "ScreenSpace.cpp"
    "Buffer.cpp"
    "VertexArray.cpp"
    "Texture.cpp"
    "TextureView.cpp"
    "TextureAtlas.cpp"
    "Sampler.cpp"
    "Shader.cpp"
    "Sprite/Sprite.cpp"
    "Sprite/SpriteDrawList.cpp"
    "Sprite/SpriteRenderer.cpp"
    "Sprite/SpriteAnimationList.cpp"
)

list(TRANSFORM SOURCE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/" PREFIX "Source Files" FILES ${SOURCE_FILES})

set(RESOURCE_FILES
    "Shaders/Sprite.shader"
)

list(TRANSFORM RESOURCE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/../../Deploy/Data/Engine/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/../../Deploy/Data/Engine/" PREFIX "Resource Files" FILES ${RESOURCE_FILES})

#
# Library
#

project(Graphics)
add_library(Graphics ${INCLUDE_FILES} ${SOURCE_FILES} ${RESOURCE_FILES})
target_compile_features(Graphics PUBLIC cxx_std_17)
target_include_directories(Graphics PUBLIC "../../Include" "../../Source")
target_precompile_headers(Graphics PRIVATE "Precompiled.hpp")
set_property(TARGET Graphics PROPERTY FOLDER "Engine")

#
# Dependencies
#

add_subdirectory("../Core" "Core")
target_link_libraries(Graphics PRIVATE Core)

add_subdirectory("../System" "System")
target_link_libraries(Graphics PRIVATE System)

add_subdirectory("../Script" "Script")
target_link_libraries(Graphics PRIVATE Script)

#
# Externals
#

# todo: should be moved to where OpenGL renderer is
if(NOT EMSCRIPTEN)
    find_package(OpenGL REQUIRED)
    target_link_libraries(Graphics ${OPENGL_GL_LIBRARY})
else()
    target_compile_options(Graphics PUBLIC "SHELL: -s FULL_ES3=1")
    target_link_options(Graphics PUBLIC "SHELL: -s FULL_ES3=1")
endif()
