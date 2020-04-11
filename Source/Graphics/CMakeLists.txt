#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
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
add_library(Graphics STATIC ${INCLUDE_FILES} ${SOURCE_FILES} ${RESOURCE_FILES})
target_compile_features(Graphics PUBLIC cxx_std_17)
target_include_directories(Graphics PUBLIC "../../Include")
target_precompile_headers(Graphics PRIVATE "Precompiled.hpp")

#
# Dependencies
#

add_subdirectory("../Common" "Common")
target_link_libraries(Graphics PRIVATE Common)

add_subdirectory("../System" "System")
target_link_libraries(Graphics PRIVATE System)

add_subdirectory("../Script" "Script")
target_link_libraries(Graphics PRIVATE Script)

#
# External
#

add_subdirectory("../../External/zlib-1.2.11" "External/zlib" EXCLUDE_FROM_ALL)
target_include_directories(Graphics PUBLIC "../../External/zlib-1.2.11")
target_include_directories(Graphics PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/External/zlib")
target_link_libraries(Graphics PRIVATE "zlibstatic")
set_property(TARGET "zlibstatic" PROPERTY PROJECT_LABEL "zlib")
set_property(TARGET "zlibstatic" PROPERTY FOLDER "External")

add_subdirectory("../../External/libpng-1.6.34" "External/libpng")
target_include_directories("png_static" PRIVATE "../../External/zlib-1.2.11")
target_include_directories("png_static" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/External/zlib")
target_include_directories(Graphics PUBLIC "../../External/libpng-1.6.34")
target_include_directories(Graphics PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/External/libpng")
target_link_libraries(Graphics PRIVATE "png_static")
set_property(TARGET "png_static" PROPERTY PROJECT_LABEL "libpng")
set_property(TARGET "png_static" PROPERTY FOLDER "External")
set_property(TARGET "genfiles" PROPERTY PROJECT_LABEL "libpngconf")
set_property(TARGET "genfiles" PROPERTY FOLDER "External")
