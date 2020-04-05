#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Library
#

project(Logger)
add_library(Logger STATIC
    "../../Include/Logger/Logger.hpp"
    "../../Include/Logger/Message.hpp"
    "../../Include/Logger/Sink.hpp"
    "../../Include/Logger/Format.hpp"
    "../../Include/Logger/Output.hpp"
    "Precompiled.hpp"
    "Logger.cpp"
    "Message.cpp"
    "Sink.cpp"
    "Format.cpp"
    "Output.cpp"
)

target_compile_features(Logger PUBLIC cxx_std_17)
target_include_directories(Logger PUBLIC "../../Include")
target_precompile_headers(Logger PRIVATE "Precompiled.hpp")

#
# External
#

add_subdirectory("../../External/fmt-6.1.2" "External/fmt" EXCLUDE_FROM_ALL)
target_include_directories(Logger PUBLIC "../../External/fmt-6.1.2/include")
target_link_libraries(Logger PUBLIC fmt::fmt)
set_property(TARGET "fmt" PROPERTY PROJECT_LABEL "fmt")
set_property(TARGET "fmt" PROPERTY FOLDER "External")
