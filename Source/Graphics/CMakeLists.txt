#
# Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Files
#

set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/Include/Graphics")
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

set(FILES_RESOURCES
    "${PROJECT_SOURCE_DIR}/Deploy/Data/Engine/Shaders/Sprite.shader"
)

set(FILES_SPRITE
    "${INCLUDE_DIR}/Sprite/Sprite.hpp"
    "${INCLUDE_DIR}/Sprite/SpriteDrawList.hpp"
    "${INCLUDE_DIR}/Sprite/SpriteRenderer.hpp"
    "${INCLUDE_DIR}/Sprite/SpriteAnimationList.hpp"
    "${SOURCE_DIR}/Sprite/Sprite.cpp"
    "${SOURCE_DIR}/Sprite/SpriteDrawList.cpp"
    "${SOURCE_DIR}/Sprite/SpriteRenderer.cpp"
    "${SOURCE_DIR}/Sprite/SpriteAnimationList.cpp"
)

set(FILES_OBJECTS
    "${INCLUDE_DIR}/ScreenSpace.hpp"
    "${INCLUDE_DIR}/Buffer.hpp"
    "${INCLUDE_DIR}/VertexArray.hpp"
    "${INCLUDE_DIR}/Texture.hpp"
    "${INCLUDE_DIR}/TextureView.hpp"
    "${INCLUDE_DIR}/TextureAtlas.hpp"
    "${INCLUDE_DIR}/Sampler.hpp"
    "${INCLUDE_DIR}/Shader.hpp"
    "${SOURCE_DIR}/ScreenSpace.cpp"
    "${SOURCE_DIR}/Buffer.cpp"
    "${SOURCE_DIR}/VertexArray.cpp"
    "${SOURCE_DIR}/Texture.cpp"
    "${SOURCE_DIR}/TextureView.cpp"
    "${SOURCE_DIR}/TextureAtlas.cpp"
    "${SOURCE_DIR}/Sampler.cpp"
    "${SOURCE_DIR}/Shader.cpp"
)

set(FILES_CONTEXT
    "${INCLUDE_DIR}/RenderContext.hpp"
    "${INCLUDE_DIR}/RenderState.hpp"
    "${SOURCE_DIR}/RenderContext.cpp"
    "${SOURCE_DIR}/RenderState.cpp"
)

source_group("Resources" FILES ${FILES_RESOURCES})
source_group("Sprite" FILES ${FILES_SPRITE})
source_group("Objects" FILES ${FILES_OBJECTS})
source_group("Context" FILES ${FILES_CONTEXT})

#
# Library
#

add_library(Graphics
    ${FILES_RESOURCES}
    ${FILES_SPRITE}
    ${FILES_OBJECTS}
    ${FILES_CONTEXT}
)

set_property(TARGET Graphics PROPERTY FOLDER "Engine")
target_compile_features(Graphics PUBLIC cxx_std_17)
target_include_directories(Graphics PUBLIC
    "${PROJECT_SOURCE_DIR}/Include"
    "${PROJECT_SOURCE_DIR}/Source"
)

target_sources(Graphics PRIVATE "${SOURCE_DIR}/Precompiled.hpp")
target_precompile_headers(Graphics PRIVATE "${SOURCE_DIR}/Precompiled.hpp")
source_group("Generated\\Precompiled" REGULAR_EXPRESSION "cmake_pch")
source_group("" FILES "${SOURCE_DIR}/Precompiled.hpp")

#
# Dependencies
#

add_subdirectory("../Core" "Core")
target_link_libraries(Graphics PRIVATE Core)

add_subdirectory("../System" "System")
target_link_libraries(Graphics PRIVATE System)

add_subdirectory("../Script" "Script")
target_link_libraries(Graphics PRIVATE Script)

enable_reflection(Graphics ${INCLUDE_DIR} ${SOURCE_DIR})
