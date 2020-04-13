#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Files
#

set(INCLUDE_FILES
    "GameFramework.hpp"
    "GameState.hpp"
    "UpdateTimer.hpp"
    "EventRouter.hpp"
    "EntityHandle.hpp"
    "EntitySystem.hpp"
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

list(TRANSFORM INCLUDE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/../../Include/Game/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/../../Include/Game/" PREFIX "Header Files" FILES ${INCLUDE_FILES})

set(SOURCE_FILES
    "Precompiled.hpp"
    "GameFramework.cpp"
    "GameState.cpp"
    "UpdateTimer.cpp"
    "EventRouter.cpp"
    "EntitySystem.cpp"
    "ComponentSystem.cpp"
    "Components/TransformComponent.cpp"
    "Components/CameraComponent.cpp"
    "Components/SpriteComponent.cpp"
    "Components/SpriteAnimationComponent.cpp"
    "Systems/IdentitySystem.cpp"
    "Systems/InterpolationSystem.cpp"
    "Systems/SpriteSystem.cpp"
)

list(TRANSFORM SOURCE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/" PREFIX "Source Files" FILES ${SOURCE_FILES})

#
# Library
#

project(Game)
add_library(Game ${INCLUDE_FILES} ${SOURCE_FILES})
target_compile_features(Game PUBLIC cxx_std_17)
target_include_directories(Game PUBLIC "../../Include")
target_precompile_headers(Game PRIVATE "Precompiled.hpp")

#
# Dependencies
#

add_subdirectory("../Core" "Core")
target_link_libraries(Game PRIVATE Core)

add_subdirectory("../System" "System")
target_link_libraries(Game PRIVATE System)

add_subdirectory("../Renderer" "Renderer")
target_link_libraries(Game PRIVATE Renderer)
