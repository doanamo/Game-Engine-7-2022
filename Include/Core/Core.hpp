/*
    Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

/*
    Core Header

    Contains includes/defines that will be part of precompiled header for every project.
*/

#include <Common/Common.hpp>

#include <fast_float/fast_float.h>

#include <zlib.h>
#include <png.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>

#ifdef PLATFORM_EMSCRIPTEN
    #include <emscripten.h>
    #define SIMDJSON_NO_PORTABILITY_WARNING
#endif

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <simdjson.h>
#define SIMDJSON_EXCEPTIONS 0

#ifdef CONFIG_DEBUG
    #define lua_assert(expression) ASSERT(expression)
#endif

#include <lua.hpp>

#include <imgui.h>
#include <imgui_stdlib.h>

#include "Script/ScriptBinding.hpp"
