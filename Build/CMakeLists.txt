#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

# Minimum required CMake version.
cmake_minimum_required(VERSION 3.12)

#
# Config
#

# Configurable variables.
set(PROJECT_NAME "GameEngine")
set(TARGET_NAME "Engine")
set(ENABLE_TESTS ON)

#
# Files
#

# List of include files.
set(INCLUDE_FILES
    "Engine.hpp"
	
	"Engine/Root.hpp"
	"Engine/Public.hpp"
	
	"Common/Build.hpp"
	"Common/Debug.hpp"
	"Common/NonCopyable.hpp"
    "Common/ScopeGuard.hpp"
	"Common/Utility.hpp"
	"Common/LinkedList.hpp"
	
	"Event/Delegate.hpp"
    "Event/Collector.hpp"
    "Event/Dispatcher.hpp"
    "Event/Receiver.hpp"
    "Event/Queue.hpp"
    "Event/Broker.hpp"
	
	"Logger/Logger.hpp"
	"Logger/Message.hpp"
    "Logger/Sink.hpp"
    "Logger/Format.hpp"
    "Logger/Output.hpp"
	
	"System/Platform.hpp"
    "System/FileSystem.hpp"
    "System/Window.hpp"
    "System/Timer.hpp"
    "System/InputDefinitions.hpp"
    "System/InputManager.hpp"
    "System/ResourcePool.hpp"
    "System/ResourceManager.hpp"
	
    "Scripting/ScriptState.hpp"
	
    "Graphics/RenderContext.hpp"
    "Graphics/RenderState.hpp"
    "Graphics/ScreenSpace.hpp"
    "Graphics/Buffer.hpp"
    "Graphics/VertexArray.hpp"
    "Graphics/Texture.hpp"
    "Graphics/TextureView.hpp"
    "Graphics/TextureAtlas.hpp"
    "Graphics/Sampler.hpp"
    "Graphics/Shader.hpp"
    "Graphics/Sprite/Sprite.hpp"
    "Graphics/Sprite/SpriteDrawList.hpp"
    "Graphics/Sprite/SpriteRenderer.hpp"
    "Graphics/Sprite/SpriteAnimationList.hpp"
	
    "Game/GameFramework.hpp"
    "Game/GameState.hpp"
    "Game/UpdateTimer.hpp"
    "Game/EventRouter.hpp"
    "Game/EntityHandle.hpp"
    "Game/EntitySystem.hpp"
    "Game/Component.hpp"
    "Game/ComponentPool.hpp"
    "Game/ComponentSystem.hpp"
    "Game/Components/TransformComponent.hpp"
    "Game/Components/CameraComponent.hpp"
    "Game/Components/SpriteComponent.hpp"
    "Game/Components/SpriteAnimationComponent.hpp"
    "Game/Systems/IdentitySystem.hpp"
    "Game/Systems/InterpolationSystem.hpp"
    "Game/Systems/SpriteSystem.hpp"
	
    "Renderer/StateRenderer.hpp"
	
    "Editor/EditorSystem.hpp"
    "Editor/EditorShell.hpp"
    "Editor/EditorRenderer.hpp"
    "Editor/Modules/InputManagerEditor.hpp"
    "Editor/Modules/GameStateEditor.hpp"
)

# List of source files.
set(SOURCE_FILES
    "Precompiled.hpp"
    "Precompiled.cpp"
	
    "Engine/Root.cpp"

    "Common/Build.cpp"
    "Common/Debug.cpp"
    "Common/Utility.cpp"

    "Logger/Logger.cpp"
    "Logger/Message.cpp"
    "Logger/Sink.cpp"
    "Logger/Format.cpp"
    "Logger/Output.cpp"

    "System/Platform.cpp"
    "System/FileSystem.cpp"
    "System/Window.cpp"
    "System/Timer.cpp"
    "System/InputDefinitions.cpp"
    "System/InputManager.cpp"
    "System/ResourceManager.cpp"

    "Scripting/ScriptState.cpp"

    "Graphics/RenderContext.cpp"
    "Graphics/RenderState.cpp"
    "Graphics/ScreenSpace.cpp"
    "Graphics/Buffer.cpp"
    "Graphics/VertexArray.cpp"
    "Graphics/Texture.cpp"
    "Graphics/TextureView.cpp"
    "Graphics/TextureAtlas.cpp"
    "Graphics/Sampler.cpp"
    "Graphics/Shader.cpp"
    "Graphics/Sprite/Sprite.cpp"
    "Graphics/Sprite/SpriteDrawList.cpp"
    "Graphics/Sprite/SpriteRenderer.cpp"
    "Graphics/Sprite/SpriteAnimationList.cpp"

    "Game/GameFramework.cpp"
    "Game/GameState.cpp"
    "Game/UpdateTimer.cpp"
    "Game/EventRouter.cpp"
    "Game/EntitySystem.cpp"
    "Game/ComponentSystem.cpp"
    "Game/Components/TransformComponent.cpp"
    "Game/Components/CameraComponent.cpp"
    "Game/Components/SpriteComponent.cpp"
    "Game/Components/SpriteAnimationComponent.cpp"
    "Game/Systems/IdentitySystem.cpp"
    "Game/Systems/InterpolationSystem.cpp"
    "Game/Systems/SpriteSystem.cpp"

    "Renderer/StateRenderer.cpp"

    "Editor/EditorSystem.cpp"
    "Editor/EditorShell.cpp"
    "Editor/EditorRenderer.cpp"
    "Editor/Modules/InputManagerEditor.cpp"
    "Editor/Modules/GameStateEditor.cpp"
)

# List of data files.
set(DATA_FILES
    "Engine/Shaders/Interface.shader"
    "Engine/Shaders/Sprite.shader"
)

# Prepend files with their corresponding directory paths.
list(TRANSFORM INCLUDE_FILES PREPEND "../Include/")
list(TRANSFORM SOURCE_FILES PREPEND "../Source/")
list(TRANSFORM DATA_FILES PREPEND "../Deploy/Data/")

# Create missing files from template.
set(EMPTY_INCLUDE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../Source/Empty.hpp")
set(EMPTY_SOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../Source/Empty.cpp")

set(FILE_LIST ${INCLUDE_FILES} ${SOURCE_FILES})

foreach(FILE_PATH ${FILE_LIST})
    set(FILE_PATH_ABSOLUTE "${CMAKE_CURRENT_SOURCE_DIR}/${FILE_PATH}")

    if(NOT EXISTS ${FILE_PATH_ABSOLUTE})
        get_filename_component(FILE_EXT ${FILE_PATH_ABSOLUTE} EXT)

        if(${FILE_EXT} STREQUAL ".hpp")
            configure_file(${EMPTY_INCLUDE_FILE} "${FILE_PATH_ABSOLUTE}")
        elseif(${FILE_EXT} STREQUAL ".cpp")
            configure_file(${EMPTY_INCLUDE_FILE} "${FILE_PATH_ABSOLUTE}")
        endif()
    endif()
endforeach()

#
# Target
#

# Specify available build configuration types.
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# Create project and target.
project(${PROJECT_NAME})
add_library(${TARGET_NAME} STATIC ${INCLUDE_FILES} ${SOURCE_FILES})

# Declare compiler features.
target_compile_features(${TARGET_NAME} PUBLIC cxx_std_17)

# Declare include directories.
target_include_directories(${TARGET_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../Include/")
target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../Source/")
target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/Source/")

#
# Config
#

# Run Git commands to retrieve and save information about current build version.
find_package("Git")

add_custom_target("Configure"
    COMMAND ${CMAKE_COMMAND}
        -D GIT_EXECUTABLE="${GIT_EXECUTABLE}"
        -D ENGINE_PROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}/.."
        -D GAME_PROJECT_DIR="${CMAKE_SOURCE_DIR}/.."
        -D INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/../Source/Config/BuildInfo.hpp.in"
        -D OUTPUT_FILE="${CMAKE_CURRENT_BINARY_DIR}/Source/Config/BuildInfo.hpp"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/../Source/Config/BuildInfo.cmake"
)

add_dependencies(${TARGET_NAME} "Configure")

# Add file with generated build version to engine sources.
# Only the output header file is needed for the build process.
# We have to create a dummy file as it will not be generated yet.
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/Source/Config/BuildInfo.hpp")

target_sources(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../Source/Config/BuildInfo.hpp.in")
source_group("Source\\Config" FILES "${CMAKE_CURRENT_SOURCE_DIR}/../Source/Config/BuildInfo.hpp.in")

target_sources(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../Source/Config/BuildInfo.cmake")
source_group("Source\\Config" FILES "${CMAKE_CURRENT_SOURCE_DIR}/../Source/Config/BuildInfo.cmake")

target_sources(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/Source/Config/BuildInfo.hpp")
source_group("Source\\Config" FILES "${CMAKE_CURRENT_BINARY_DIR}/Source/Config/BuildInfo.hpp")

# Write a file pointing at the engine directory.
get_filename_component(ABSOLUTE_ENGINE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Deploy" ABSOLUTE)
file(WRITE "${CMAKE_BINARY_DIR}/EngineDir.txt" ${ABSOLUTE_ENGINE_DIR})

#
# Tests
#

# Enable unit tests.
if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory("../Tests" "Tests")
endif()

#
# Platform
#

# Enable source folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

# Add engine data files to folder structure.
foreach(DATA_FILE ${DATA_FILES})
    target_sources(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/${DATA_FILE}")
endforeach()

# Keep folder structure of include/source/data files.
source_group(TREE "${PROJECT_SOURCE_DIR}/.." FILES ${INCLUDE_FILES} ${SOURCE_FILES} ${DATA_FILES})

# Set target as default project for Visual Studio.
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})

# Compiler specific settings.
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # Disable Standard C++ Library warnings.
    target_compile_definitions(${TARGET_NAME} PUBLIC "_CRT_SECURE_NO_WARNINGS")
    target_compile_definitions(${TARGET_NAME} PUBLIC "_SCL_SECURE_NO_WARNINGS")
    
    # Use precompiled header for quicker compilation of static headers.
    set_source_files_properties(${SOURCE_FILES} PROPERTIES COMPILE_FLAGS
		"/Yu\"Precompiled.hpp\" /Fp\"${CMAKE_CURRENT_BINARY_DIR}/Source/Precompiled-$(Configuration).pch\"")

    set_source_files_properties("../Source/Precompiled.cpp" PROPERTIES COMPILE_FLAGS
		"/Yc\"Precompiled.hpp\" /Fp\"${CMAKE_CURRENT_BINARY_DIR}/Source/Precompiled-$(Configuration).pch\"")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # Enable additional warnings.
    target_compile_options(${TARGET_NAME} PUBLIC "-Wall")
endif()

#
# External
#

# Add utfcpp library.
target_include_directories(${TARGET_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../External/utfcpp-2.3.6/source")

# Add GLM library.
target_include_directories(${TARGET_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../External/GLM-0.9.9.3")

# Add OpenGL library.
find_package(OpenGL REQUIRED)
target_link_libraries(${TARGET_NAME} ${OPENGL_GL_LIBRARY})

# Add GLEW library.
add_subdirectory("../External/GLEW-2.1.0/build/cmake" "External/GLEW" EXCLUDE_FROM_ALL)
target_include_directories(${TARGET_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../External/GLEW-2.1.0/include")
target_link_libraries(${TARGET_NAME} "glew_s")

set_property(TARGET "glew_s" PROPERTY PROJECT_LABEL "GLEW")
set_property(TARGET "glew_s" PROPERTY FOLDER "External")

# Add GLFW library.
add_subdirectory("../External/GLFW-3.2.1" "External/GLFW" EXCLUDE_FROM_ALL)
target_include_directories(${TARGET_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../External/GLFW-3.2.1/include")
target_link_libraries(${TARGET_NAME} "glfw")

set_property(TARGET "glfw" PROPERTY PROJECT_LABEL "GLFW")
set_property(TARGET "glfw" PROPERTY FOLDER "External")

# Add zlib library.
add_subdirectory("../External/zlib-1.2.11" "External/zlib" EXCLUDE_FROM_ALL)
target_include_directories(${TARGET_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../External/zlib-1.2.11")
target_include_directories(${TARGET_NAME} PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/External/zlib")
target_link_libraries(${TARGET_NAME} "zlibstatic")

set_property(TARGET "zlibstatic" PROPERTY PROJECT_LABEL "zlib")
set_property(TARGET "zlibstatic" PROPERTY FOLDER "External")

# Add libpng library.
add_subdirectory("../External/libpng-1.6.34" "External/libpng" EXCLUDE_FROM_ALL)
target_include_directories("png_static" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../External/zlib-1.2.11")
target_include_directories("png_static" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/External/zlib")
target_include_directories(${TARGET_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../External/libpng-1.6.34")
target_include_directories(${TARGET_NAME} PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/External/libpng")
target_link_libraries(${TARGET_NAME} "png_static")

set_property(TARGET "png_static" PROPERTY PROJECT_LABEL "libpng")
set_property(TARGET "png_static" PROPERTY FOLDER "External")
set_property(TARGET "genfiles" PROPERTY PROJECT_LABEL "libpngconf")
set_property(TARGET "genfiles" PROPERTY FOLDER "External")

# Add Lua library.
add_subdirectory("../External/Lua-5.3.5" "External/Lua" EXCLUDE_FROM_ALL)
target_link_libraries(${TARGET_NAME} "lua_static")

set_property(TARGET "lua_static" PROPERTY PROJECT_LABEL "Lua")
set_property(TARGET "lua_static" PROPERTY FOLDER "External")

# Add ImGui library.
add_subdirectory("../External/ImGui-1.62" "External/ImGui" EXCLUDE_FROM_ALL)
target_include_directories("ImGui" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../External/GLEW-2.1.0/include")
target_link_libraries(${TARGET_NAME} "ImGui")

set_property(TARGET "ImGui" PROPERTY FOLDER "External")
