#
# Copyright (c) 2018 Piotr Doan. All rights reserved.
#

# Minimum required CMake version.
cmake_minimum_required(VERSION 3.10)

#
# Config
#

# Configurable variables.
set(PROJECT_NAME "GameEngine")
set(TARGET_NAME "Engine")
set(ENABLE_TESTS ON)

#
# Sources
#

# List of source files.
set(SOURCE_FILES
    "Include/EnginePublic.hpp"
    "Source/Precompiled.hpp"
    "Source/Precompiled.cpp"
    "Include/Engine.hpp"
    "Source/Engine.cpp"

    "Include/Common/Build.hpp"
    "Source/Common/Build.cpp"
    "Include/Common/Debug.hpp"
    "Source/Common/Debug.cpp"
    "Include/Common/NonCopyable.hpp"
    "Include/Common/ScopeGuard.hpp"
    "Include/Common/Utility.hpp"
    "Source/Common/Utility.cpp"
    "Include/Common/LinkedList.hpp"
    "Include/Common/Event/Delegate.hpp"
    "Include/Common/Event/Collector.hpp"
    "Include/Common/Event/Dispatcher.hpp"
    "Include/Common/Event/Receiver.hpp"

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
    "Include/System/InputMapping.hpp"
    "Source/System/InputMapping.cpp"
    "Include/System/FileSystem.hpp"
    "Source/System/FileSystem.cpp"
    "Include/System/Window.hpp"
    "Source/System/Window.cpp"
    "Include/System/Timer.hpp"
    "Source/System/Timer.cpp"
    "Include/System/UpdateTimer.hpp"
    "Source/System/UpdateTimer.cpp"
    "Include/System/InputState.hpp"
    "Source/System/InputState.cpp"
    "Include/System/ResourcePool.hpp"
    "Include/System/ResourceManager.hpp"
    "Source/System/ResourceManager.cpp"

    "Include/Scripting/ScriptState.hpp"
    "Source/Scripting/ScriptState.cpp"

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
    "Include/Graphics/TextureView.hpp"
    "Source/Graphics/TextureView.cpp"
    "Include/Graphics/TextureAtlas.hpp"
    "Source/Graphics/TextureAtlas.cpp"
    "Include/Graphics/Sampler.hpp"
    "Source/Graphics/Sampler.cpp"
    "Include/Graphics/Shader.hpp"
    "Source/Graphics/Shader.cpp"
    "Include/Graphics/Sprite/Sprite.hpp"
    "Source/Graphics/Sprite/Sprite.cpp"
    "Include/Graphics/Sprite/SpriteDrawList.hpp"
    "Source/Graphics/Sprite/SpriteDrawList.cpp"
    "Include/Graphics/Sprite/SpriteRenderer.hpp"
    "Source/Graphics/Sprite/SpriteRenderer.cpp"
    "Include/Graphics/Sprite/SpriteAnimationList.hpp"
    "Source/Graphics/Sprite/SpriteAnimationList.cpp"

    "Include/Game/EntityHandle.hpp"
    "Include/Game/EntitySystem.hpp"
    "Source/Game/EntitySystem.cpp"
    "Include/Game/Component.hpp"
    "Include/Game/ComponentPool.hpp"
    "Include/Game/ComponentSystem.hpp"
    "Source/Game/ComponentSystem.cpp"
    "Include/Game/GameState.hpp"
    "Source/Game/GameState.cpp"
    "Include/Game/Components/TransformComponent.hpp"
    "Source/Game/Components/TransformComponent.cpp"
    "Include/Game/Components/CameraComponent.hpp"
    "Source/Game/Components/CameraComponent.cpp"
    "Include/Game/Components/SpriteComponent.hpp"
    "Source/Game/Components/SpriteComponent.cpp"
    "Include/Game/Components/SpriteAnimationComponent.hpp"
    "Source/Game/Components/SpriteAnimationComponent.cpp"
    "Include/Game/Systems/IdentitySystem.hpp"
    "Source/Game/Systems/IdentitySystem.cpp"
    "Include/Game/Systems/InterpolationSystem.hpp"
    "Source/Game/Systems/InterpolationSystem.cpp"
    "Include/Game/Systems/SpriteSystem.hpp"
    "Source/Game/Systems/SpriteSystem.cpp"

    "Include/Renderer/StateRenderer.hpp"
    "Source/Renderer/StateRenderer.cpp"

    "Include/Editor/EditorSystem.hpp"
    "Source/Editor/EditorSystem.cpp"
    "Include/Editor/GameStateEditor.hpp"
    "Source/Editor/GameStateEditor.cpp"
)

# Create missing source files from template.
foreach(SOURCE_FILE ${SOURCE_FILES})
    set(SOURCE_FILE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE}")

    if(NOT EXISTS ${SOURCE_FILE_PATH})
        get_filename_component(SOURCE_FILE_EXT ${SOURCE_FILE_PATH} EXT)

        if(${SOURCE_FILE_EXT} STREQUAL ".hpp")
            configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Source/Empty.hpp" "${SOURCE_FILE_PATH}")
        elseif(${SOURCE_FILE_EXT} STREQUAL ".cpp")
            configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Source/Empty.cpp" "${SOURCE_FILE_PATH}")
        endif()
    endif()
endforeach()

#
# Target
#

# Create project and target.
project(${PROJECT_NAME})
add_library(${TARGET_NAME} STATIC ${SOURCE_FILES})

# Declare compiler features.
target_compile_features(${TARGET_NAME} PUBLIC cxx_std_17)

# Declare include directories.
target_include_directories(${TARGET_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Include")
target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Source")
target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/Source")

#
# Config
#

# Run Git commands to retrieve and save information about current build version.
find_package("Git")

add_custom_target("Configure"
    COMMAND ${CMAKE_COMMAND}
        -D GIT_EXECUTABLE="${GIT_EXECUTABLE}"
        -D ENGINE_PROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
        -D GAME_PROJECT_DIR="${CMAKE_SOURCE_DIR}"
        -D INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/Source/Config/BuildInfo.hpp.in"
        -D OUTPUT_FILE="${CMAKE_CURRENT_BINARY_DIR}/Source/Config/BuildInfo.hpp"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/Source/Config/BuildInfo.cmake"
)

add_dependencies(${TARGET_NAME} "Configure")

# Add file with generated build version to engine sources.
# Only the output header file is needed for the build process.
# We have to create a dummy file as it will not be generated yet.
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/Source/Config/BuildInfo.hpp")

target_sources(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Source/Config/BuildInfo.hpp.in")
source_group("Source\\Config" FILES "${CMAKE_CURRENT_SOURCE_DIR}/Source/Config/BuildInfo.hpp.in")

target_sources(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Source/Config/BuildInfo.cmake")
source_group("Source\\Config" FILES "${CMAKE_CURRENT_SOURCE_DIR}/Source/Config/BuildInfo.cmake")

target_sources(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/Source/Config/BuildInfo.hpp")
source_group("Source\\Config" FILES "${CMAKE_CURRENT_BINARY_DIR}/Source/Config/BuildInfo.hpp")

# Write a file pointing at the engine directory.
get_filename_component(ABSOLUTE_ENGINE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Deploy" ABSOLUTE)
file(WRITE "${CMAKE_BINARY_DIR}/EngineDir.txt" "${ABSOLUTE_ENGINE_DIR}/")

#
# Data
#

# Add shader files to solution's folder structure.
target_sources(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/Data/Engine/Shaders/Interface.shader")
source_group("Data\\Shaders" FILES "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/Data/Engine/Shaders/Interface.shader")

target_sources(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/Data/Engine/Shaders/Sprite.shader")
source_group("Data\\Shaders" FILES "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/Data/Engine/Shaders/Sprite.shader")

#
# Tests
#

# Enable unit tests.
if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory("Tests")
endif()

#
# Platform
#

# Enable source folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

# Keep folder structure of include and source files.
source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCE_FILES})

# Compiler specific settings.
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # Disable Standard C++ Library warnings.
    target_compile_definitions(${TARGET_NAME} PUBLIC "_CRT_SECURE_NO_WARNINGS")
    target_compile_definitions(${TARGET_NAME} PUBLIC "_SCL_SECURE_NO_WARNINGS")
    
    # Use precompiled header for quicker compilation of static headers.
    set_source_files_properties(${SOURCE_FILES} PROPERTIES
        COMPILE_FLAGS "/Yu\"Precompiled.hpp\" /Fp\"${CMAKE_CURRENT_BINARY_DIR}/Source/Precompiled-$(Configuration).pch\""
    )

    set_source_files_properties("Source/Precompiled.cpp" PROPERTIES
        COMPILE_FLAGS "/Yc\"Precompiled.hpp\" /Fp\"${CMAKE_CURRENT_BINARY_DIR}/Source/Precompiled-$(Configuration).pch\""
    )
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # Enable additional warnings.
    target_compile_options(${TARGET_NAME} PUBLIC "-Wall")
endif()

#
# External
#

# Add utfcpp library.
target_include_directories(${TARGET_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/External/utfcpp-2.3.6/source")

# Add GLM library.
target_include_directories(${TARGET_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/External/GLM-0.9.9.3")

# Add OpenGL library.
find_package(OpenGL REQUIRED)
target_link_libraries(${TARGET_NAME} ${OPENGL_GL_LIBRARY})

# Add GLEW library.
add_subdirectory("External/GLEW-2.1.0/build/cmake" "External/GLEW" EXCLUDE_FROM_ALL)
target_include_directories(${TARGET_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/External/GLEW-2.1.0/include")
target_link_libraries(${TARGET_NAME} "glew_s")

set_property(TARGET "glew_s" PROPERTY PROJECT_LABEL "GLEW")
set_property(TARGET "glew_s" PROPERTY FOLDER "External")

# Add GLFW library.
add_subdirectory("External/GLFW-3.2.1" "External/GLFW" EXCLUDE_FROM_ALL)
target_include_directories(${TARGET_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/External/GLFW-3.2.1/include")
target_link_libraries(${TARGET_NAME} "glfw")

set_property(TARGET "glfw" PROPERTY PROJECT_LABEL "GLFW")
set_property(TARGET "glfw" PROPERTY FOLDER "External")

# Add zlib library.
add_subdirectory("External/zlib-1.2.11" "External/zlib" EXCLUDE_FROM_ALL)
target_include_directories(${TARGET_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/External/zlib-1.2.11")
target_include_directories(${TARGET_NAME} PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/External/zlib")
target_link_libraries(${TARGET_NAME} "zlibstatic")

set_property(TARGET "zlibstatic" PROPERTY PROJECT_LABEL "zlib")
set_property(TARGET "zlibstatic" PROPERTY FOLDER "External")

# Add libpng library.
add_subdirectory("External/libpng-1.6.34" "External/libpng" EXCLUDE_FROM_ALL)
target_include_directories("png_static" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/External/zlib-1.2.11")
target_include_directories("png_static" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/External/zlib")
target_include_directories(${TARGET_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/External/libpng-1.6.34")
target_include_directories(${TARGET_NAME} PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/External/libpng")
target_link_libraries(${TARGET_NAME} "png_static")

set_property(TARGET "png_static" PROPERTY PROJECT_LABEL "libpng")
set_property(TARGET "png_static" PROPERTY FOLDER "External")
set_property(TARGET "genfiles" PROPERTY PROJECT_LABEL "libpngconf")
set_property(TARGET "genfiles" PROPERTY FOLDER "External")

# Add Lua library.
add_subdirectory("External/Lua-5.3.5" "External/Lua" EXCLUDE_FROM_ALL)
target_link_libraries(${TARGET_NAME} "lua_static")

set_property(TARGET "lua_static" PROPERTY PROJECT_LABEL "Lua")
set_property(TARGET "lua_static" PROPERTY FOLDER "External")

# Add ImGui library.
add_subdirectory("External/ImGui-1.62" "External/ImGui" EXCLUDE_FROM_ALL)
target_include_directories("ImGui" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/External/GLEW-2.1.0/include")
target_link_libraries(${TARGET_NAME} "ImGui")

set_property(TARGET "ImGui" PROPERTY FOLDER "External")
