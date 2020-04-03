#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

cmake_minimum_required(VERSION 3.16)

#
# Library
#

add_library(Common STATIC
    "../../Include/Common/Utility.hpp"
    "../../Include/Common/Debug.hpp"
    "../../Include/Common/NonCopyable.hpp"
    "../../Include/Common/ScopeGuard.hpp"
    "../../Include/Common/LinkedList.hpp"
    "../../Include/Common/StateMachine.hpp"
    
    "Precompiled.hpp"
    "Utility.cpp"
    "Debug.cpp"
)

target_compile_features(Common PUBLIC cxx_std_17)
target_include_directories(Common PUBLIC "../../Include")
target_include_directories(Common PRIVATE "../../Include/Common")
target_precompile_headers(Common PRIVATE "Precompiled.hpp")

#
# Dependencies
#

add_subdirectory("../Logger" "Logger" EXCLUDE_FROM_ALL)
target_link_libraries(Common PUBLIC Logger)

#
# Externals
#

target_include_directories(Common PUBLIC "../../External/utfcpp-2.3.6/source")
