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
    "GameFramework.hpp"
    "GameState.hpp"
    "GameInstance.hpp"
    "GameSystem.hpp"
    "EntityHandle.hpp"
    "EntitySystem.hpp"
    "TickTimer.hpp"
    "Component.hpp"
    "ComponentPool.hpp"
    "ComponentSystem.hpp"
    "Components/TransformComponent.hpp"
    "Components/CameraComponent.hpp"
    "Components/SpriteComponent.hpp"
    "Components/SpriteAnimationComponent.hpp"
    "Systems/IdentitySystem.hpp"
    "Systems/InterpolationSystem.hpp"
    "Systems/SpriteSystem.hpp"
)

set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../Include/Game/")
list(TRANSFORM INCLUDE_FILES PREPEND ${INCLUDE_DIR})
source_group(TREE ${INCLUDE_DIR} PREFIX "Header Files" FILES ${INCLUDE_FILES})

set(SOURCE_FILES
    "Precompiled.hpp"
    "GameFramework.cpp"
    "GameInstance.cpp"
    "EntitySystem.cpp"
    "TickTimer.cpp"
    "ComponentSystem.cpp"
    "Components/TransformComponent.cpp"
    "Components/CameraComponent.cpp"
    "Components/SpriteComponent.cpp"
    "Components/SpriteAnimationComponent.cpp"
    "Systems/IdentitySystem.cpp"
    "Systems/InterpolationSystem.cpp"
    "Systems/SpriteSystem.cpp"
)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/")
list(TRANSFORM SOURCE_FILES PREPEND ${SOURCE_DIR})
source_group(TREE ${SOURCE_DIR} PREFIX "Source Files" FILES ${SOURCE_FILES})

#
# Library
#

project(Game)
add_library(Game ${INCLUDE_FILES} ${SOURCE_FILES})
target_compile_features(Game PUBLIC cxx_std_17)
target_include_directories(Game PUBLIC "../../Include" "../../Source")
target_precompile_headers(Game PRIVATE "Precompiled.hpp")
set_property(TARGET Game PROPERTY FOLDER "Engine")

#
# Dependencies
#

add_subdirectory("../Core" "Core")
target_link_libraries(Game PRIVATE Core)

add_subdirectory("../Graphics" "Graphics")
target_link_libraries(Game PRIVATE Graphics)

add_subdirectory("../Renderer" "Renderer")
target_link_libraries(Game PRIVATE Renderer)

enable_reflection(Game ${INCLUDE_DIR} ${SOURCE_DIR})
