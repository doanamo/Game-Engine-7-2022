/*
    Copyright (c) 2018-2022 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

/*
    Common Header

    Contains includes/defines that will be part of precompiled header for every project.
*/

#include <cstdint>
#include <cctype>
#include <ctime>

#include <thread>
#include <mutex>
#include <memory>
#include <numeric>
#include <charconv>
#include <functional>
#include <algorithm>
#include <random>
#include <utility>

#include <any>
#include <tuple>
#include <optional>
#include <variant>
#include <string>
#include <vector>
#include <deque>
#include <queue>
#include <stack>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <sstream>

#include <filesystem>
namespace fs = std::filesystem;

#include <utf8/core.h>
#include <utf8/unchecked.h>

#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/chrono.h>

#include "Common/Defines.hpp"
#include "Common/Debug/Debug.hpp"
#include "Common/Profiling/Profiling.hpp"
#include "Common/Utility/Utility.hpp"
#include "Common/Utility/NonCopyable.hpp"
#include "Common/Utility/Resettable.hpp"
#include "Common/Utility/ScopeGuard.hpp"
#include "Common/Utility/Delegate.hpp"
#include "Common/Utility/Result.hpp"
#include "Common/Utility/Name.hpp"
#include "Common/Logger/Logger.hpp"

#include <Reflection/Reflection.hpp>
