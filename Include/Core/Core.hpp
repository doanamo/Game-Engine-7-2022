/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

/*
    Common Includes
*/

#include <cctype>
#include <ctime>
#include <cstdint>

#include <memory>
#include <algorithm>
#include <functional>
#include <filesystem>
#include <typeindex>

#include <sstream>
#include <fstream>
#include <iostream>

#include <string>
#include <queue>
#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace fs = std::filesystem;

#include <utf8/core.h>
#include <utf8/unchecked.h>
#include <fmt/core.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif

#include <imgui.h>
#include <imgui_stdlib.h>

#include <Common/Debug.hpp>
#include <Common/Utility.hpp>
#include <Common/NonCopyable.hpp>
#include <Common/Resettable.hpp>
#include <Common/ScopeGuard.hpp>
#include <Common/Result.hpp>
#include <Common/Logger/Logger.hpp>
#include <Reflection/Reflection.hpp>
