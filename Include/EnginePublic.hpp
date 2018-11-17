/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

// Standard
#include <cmath>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>

// Windows
#ifdef WIN32
    #define WIN32_LEAD_AND_MEAN
    #define NOMINMAX
    #include <windows.h>
#endif

// utfcpp
#include <utf8/core.h>
#include <utf8/unchecked.h>

// GLM
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// zlib
#include <zlib.h>

// libpng
#include <png.h>

// Lua
#include <lua.hpp>

// ImGUI
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// Project
#include "Common/Build.hpp"
#include "Common/Debug.hpp"
#include "Common/NonCopyable.hpp"
#include "Common/ScopeGuard.hpp"
#include "Common/Utility.hpp"
#include "Common/Event/Delegate.hpp"
#include "Logger/Logger.hpp"
