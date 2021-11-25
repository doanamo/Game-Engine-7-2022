@echo off
setlocal

rem Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
rem Software distributed under the permissive MIT License.

set engineDir="%~dp0/../"

set inputDir=%1
set outputDir=%2
set configType=%3

if [%inputDir%] == [] set inputDir="../."
if [%outputDir%] == [] set outputDir="CMake-Emscripten-Release"
if [%configType%] == [] set configType="Release"
if [%projectPage%] == [] set projectPage="Example/Example/Example.html"

echo ---- Checking Python availability...
python --version 2>NUL
if %ERRORLEVEL% NEQ 0 (
    echo Python could not be found!
    pause
    exit /b
)

echo ---- Preparing Emscripten SDK...
if not exist "CMake-Emscripten-SDK" (
    git clone -c advice.detachedHead=false --depth=1 --branch=3.0.0 https://github.com/emscripten-core/emsdk.git "CMake-Emscripten-SDK"
    
    cd "CMake-Emscripten-SDK"
    call emsdk.bat install latest
    call emsdk.bat activate latest >nul 2>&1
    cd "../."
)

cd "CMake-Emscripten-SDK"
call emsdk_env.bat >nul 2>&1
cd "../."

echo ---- Preparing build directory...
mkdir %outputDir% >nul 2>&1
cd %outputDir%

echo ---- Preparing Ninja build system...
if not exist "Ninja" (
    git clone -c advice.detachedHead=false --depth=1 --branch=v1.10.2 https://github.com/ninja-build/ninja.git "Ninja"
    
    mkdir "Ninja/build" >nul 2>&1
    cd "Ninja/build"
    cmake "../." -DBUILD_TESTING=0
    cmake --build . --config Release >nul 2>&1
    cd "../../."
)

echo ---- Preparing reflection generator...
mkdir "ReflectionGenerator" >nul 2>&1
cd "ReflectionGenerator"
cmake "%engineDir%/Tools/ReflectionGenerator"
cmake --build . --config Release >nul 2>&1
cd "../."

echo ---- Generating CMake project...
cmake -G "Ninja" "../%inputDir%" -DCMAKE_TOOLCHAIN_FILE="../CMake-Emscripten-SDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake" -DCMAKE_MAKE_PROGRAM="%cd%/Ninja/build/release/ninja.exe" -DCMAKE_BUILD_TYPE=%configType%

if %ERRORLEVEL% NEQ 0 (
    pause
    exit /b
)

echo ---- Building generated project...
cmake --build .

if %ERRORLEVEL% NEQ 0 (
    pause
    exit /b
)

echo ---- Opening project page...
emrun %projectPage%
