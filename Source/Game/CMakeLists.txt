#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

cmake_minimum_required(VERSION 3.16)

#
# Library
#

project(Game)
add_library(Game STATIC
    "../../Include/Game/GameFramework.hpp"
    "../../Include/Game/GameState.hpp"
    "../../Include/Game/UpdateTimer.hpp"
    "../../Include/Game/EventRouter.hpp"
    "../../Include/Game/EntityHandle.hpp"
    "../../Include/Game/EntitySystem.hpp"
    "../../Include/Game/Component.hpp"
    "../../Include/Game/ComponentPool.hpp"
    "../../Include/Game/ComponentSystem.hpp"
    "../../Include/Game/Components/TransformComponent.hpp"
    "../../Include/Game/Components/CameraComponent.hpp"
    "../../Include/Game/Components/SpriteComponent.hpp"
    "../../Include/Game/Components/SpriteAnimationComponent.hpp"
    "../../Include/Game/Systems/IdentitySystem.hpp"
    "../../Include/Game/Systems/InterpolationSystem.hpp"
    "../../Include/Game/Systems/SpriteSystem.hpp"
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

target_compile_features(Game PUBLIC cxx_std_17)
target_include_directories(Game PUBLIC "../../Include")
target_precompile_headers(Game PRIVATE "Precompiled.hpp")

#
# Dependencies
#

if(NOT TARGET Common)
    add_subdirectory("../Common" "Common" EXCLUDE_FROM_ALL)
endif()

target_link_libraries(Game PRIVATE Common)

if(NOT TARGET System)
    add_subdirectory("../System" "System" EXCLUDE_FROM_ALL)
endif()

target_link_libraries(Game PRIVATE System)

if(NOT TARGET Renderer)
    add_subdirectory("../Renderer" "Renderer" EXCLUDE_FROM_ALL)
endif()

target_link_libraries(Game PRIVATE Renderer)
