#
# Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
# Software distributed under the permissive MIT License.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Files
#

set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/Include/Game")
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

set(FILES_ENTITY
    "${INCLUDE_DIR}/EntityHandle.hpp"
    "${INCLUDE_DIR}/EntitySystem.hpp"
    "${INCLUDE_DIR}/Component.hpp"
    "${INCLUDE_DIR}/ComponentPool.hpp"
    "${INCLUDE_DIR}/ComponentSystem.hpp"
    "${SOURCE_DIR}/EntitySystem.cpp"
    "${SOURCE_DIR}/ComponentSystem.cpp"
)

set(FILES_COMPONENTS
    "${INCLUDE_DIR}/Components/TransformComponent.hpp"
    "${INCLUDE_DIR}/Components/CameraComponent.hpp"
    "${INCLUDE_DIR}/Components/SpriteComponent.hpp"
    "${INCLUDE_DIR}/Components/SpriteAnimationComponent.hpp"
    "${SOURCE_DIR}/Components/TransformComponent.cpp"
    "${SOURCE_DIR}/Components/CameraComponent.cpp"
    "${SOURCE_DIR}/Components/SpriteComponent.cpp"
    "${SOURCE_DIR}/Components/SpriteAnimationComponent.cpp"
)

set(FILES_SYSTEMS
    "${INCLUDE_DIR}/Systems/IdentitySystem.hpp"
    "${INCLUDE_DIR}/Systems/InterpolationSystem.hpp"
    "${INCLUDE_DIR}/Systems/SpriteSystem.hpp"
    "${SOURCE_DIR}/Systems/IdentitySystem.cpp"
    "${SOURCE_DIR}/Systems/InterpolationSystem.cpp"
    "${SOURCE_DIR}/Systems/SpriteSystem.cpp"
)

set(FILES_FRAMEWORK
    "${INCLUDE_DIR}/GameFramework.hpp"
    "${INCLUDE_DIR}/GameState.hpp"
    "${INCLUDE_DIR}/GameInstance.hpp"
    "${INCLUDE_DIR}/GameSystem.hpp"
    "${SOURCE_DIR}/GameFramework.cpp"
    "${SOURCE_DIR}/GameInstance.cpp"
)

set(FILES_UTILITY
    "${INCLUDE_DIR}/TickTimer.hpp"
    "${SOURCE_DIR}/TickTimer.cpp"
)

source_group("Entity" FILES ${FILES_ENTITY})
source_group("Components" FILES ${FILES_COMPONENTS})
source_group("Systems" FILES ${FILES_SYSTEMS})
source_group("Framework" FILES ${FILES_FRAMEWORK})
source_group("Utility" FILES ${FILES_UTILITY})

#
# Library
#

add_library(Game
    ${FILES_ENTITY}
    ${FILES_COMPONENTS}
    ${FILES_SYSTEMS}
    ${FILES_FRAMEWORK}
    ${FILES_UTILITY}
)

set_property(TARGET Game PROPERTY FOLDER "Engine")
target_compile_features(Game PUBLIC cxx_std_17)
target_include_directories(Game PUBLIC
    "${PROJECT_SOURCE_DIR}/Include"
    "${PROJECT_SOURCE_DIR}/Source"
)

target_sources(Game PRIVATE "${SOURCE_DIR}/Precompiled.hpp")
target_precompile_headers(Game PRIVATE "${SOURCE_DIR}/Precompiled.hpp")
source_group("Generated\\Precompiled" REGULAR_EXPRESSION "cmake_pch")
source_group("" FILES "${SOURCE_DIR}/Precompiled.hpp")

#
# Dependencies
#

add_subdirectory("../Core" "Core")
target_link_libraries(Game PRIVATE Core)

add_subdirectory("../Graphics" "Graphics")
target_link_libraries(Game PRIVATE Graphics)

enable_reflection(Game ${INCLUDE_DIR} ${SOURCE_DIR})
