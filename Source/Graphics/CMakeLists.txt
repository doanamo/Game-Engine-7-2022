#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

cmake_minimum_required(VERSION 3.16)

#
# Library
#

add_library(Graphics STATIC
    "../../Include/Graphics/RenderContext.hpp"
    "../../Include/Graphics/RenderState.hpp"
    "../../Include/Graphics/ScreenSpace.hpp"
    "../../Include/Graphics/Buffer.hpp"
    "../../Include/Graphics/VertexArray.hpp"
    "../../Include/Graphics/Texture.hpp"
    "../../Include/Graphics/TextureView.hpp"
    "../../Include/Graphics/TextureAtlas.hpp"
    "../../Include/Graphics/Sampler.hpp"
    "../../Include/Graphics/Shader.hpp"
    "../../Include/Graphics/Sprite/Sprite.hpp"
    "../../Include/Graphics/Sprite/SpriteDrawList.hpp"
    "../../Include/Graphics/Sprite/SpriteRenderer.hpp"
    "../../Include/Graphics/Sprite/SpriteAnimationList.hpp"
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

target_compile_features(Graphics PUBLIC cxx_std_17)
target_include_directories(Graphics PUBLIC "../../Include")
target_precompile_headers(Graphics PRIVATE "Precompiled.hpp")

#
# Dependencies
#

add_subdirectory("../Script" "Script" EXCLUDE_FROM_ALL)
target_link_libraries(Graphics PUBLIC Script)

#
# External
#

add_subdirectory("../../External/zlib-1.2.11" "External/zlib" EXCLUDE_FROM_ALL)
target_include_directories(Graphics PUBLIC "../../External/zlib-1.2.11")
target_include_directories(Graphics PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/External/zlib")
target_link_libraries(Graphics PRIVATE "zlibstatic")
set_property(TARGET "zlibstatic" PROPERTY PROJECT_LABEL "zlib")
set_property(TARGET "zlibstatic" PROPERTY FOLDER "External")

add_subdirectory("../../External/libpng-1.6.34" "External/libpng" EXCLUDE_FROM_ALL)
target_include_directories("png_static" PRIVATE "../../External/zlib-1.2.11")
target_include_directories("png_static" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/External/zlib")
target_include_directories(Graphics PUBLIC "../../External/libpng-1.6.34")
target_include_directories(Graphics PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/External/libpng")
target_link_libraries(Graphics PRIVATE "png_static")
set_property(TARGET "png_static" PROPERTY PROJECT_LABEL "libpng")
set_property(TARGET "png_static" PROPERTY FOLDER "External")
set_property(TARGET "genfiles" PROPERTY PROJECT_LABEL "libpngconf")
set_property(TARGET "genfiles" PROPERTY FOLDER "External")
