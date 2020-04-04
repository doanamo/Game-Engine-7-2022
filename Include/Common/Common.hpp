/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    Common Includes
*/

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

#include "Logger/Logger.hpp"
#include "Common/Debug.hpp"
#include "Common/Utility.hpp"
#include "Common/NonCopyable.hpp"
#include "Common/ScopeGuard.hpp"
#include "Reflection/Reflection.hpp"
