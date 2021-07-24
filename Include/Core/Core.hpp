/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

/*
    Core Header

    Contains common includes that will be part of precompiled header for every project.
*/

#include <cctype>
#include <ctime>
#include <cstdint>

#include <thread>
#include <memory>
#include <algorithm>
#include <functional>
#include <filesystem>
#include <typeindex>

#include <fstream>

#include <optional>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <map>
#include <unordered_set>
#include <unordered_map>

namespace fs = std::filesystem;

#include <utf8/core.h>
#include <utf8/unchecked.h>
#include <fmt/core.h>

#include <zlib.h>
#include <png.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif

#include <imgui.h>
#include <imgui_stdlib.h>

#include <Reflection/Reflection.hpp>
#include <Common/Debug.hpp>
#include <Common/Utility.hpp>
#include <Common/NonCopyable.hpp>
#include <Common/Resettable.hpp>
#include <Common/ScopeGuard.hpp>
#include <Common/Result.hpp>
#include <Common/Name.hpp>
#include <Common/Logger/Logger.hpp>
