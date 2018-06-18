/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

// Standard
#include <cctype>
#include <ctime>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>

// Windows
#ifdef WIN32
#define WIN32_LEAD_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

// GLM
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>

// GLEW
#include <gl/glew.h>

// GLFW
#include <glfw/glfw3.h>

// Project
#include "Common/Build.hpp"
#include "Common/Debug.hpp"
#include "Common/NonCopyable.hpp"
#include "Common/ScopeGuard.hpp"
#include "Common/Utility.hpp"
#include "Events/Delegate.hpp"
#include "Logger/Logger.hpp"

