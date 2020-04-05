#
# Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
#

cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

#
# Library
#

project(Event)
add_library(Event STATIC
    "../../Include/Event/Delegate.hpp"
    "../../Include/Event/Collector.hpp"
    "../../Include/Event/Dispatcher.hpp"
    "../../Include/Event/Receiver.hpp"
    "../../Include/Event/Queue.hpp"
    "../../Include/Event/Broker.hpp"
    "Empty.cpp"
)

target_compile_features(Event PUBLIC cxx_std_17)
target_include_directories(Event PUBLIC "../../Include")
