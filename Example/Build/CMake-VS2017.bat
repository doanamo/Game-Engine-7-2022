@echo off

rem Copyright (c) 2018-2019 Piotr Doan. All rights reserved.
echo -- Generating CMake project...

rem Collect command line arguments.
set outputDir=%1
set inputDir=%2

if [%outputDir%] == [] set outputDir="CMake-VS2017"
if [%inputDir%] == [] set inputDir="../"

rem Create empty project directory.
mkdir %outputDir% >nul 2>&1
cd %outputDir%

rem Generate solution files.
cmake -G "Visual Studio 15" ../%inputDir%

if %ERRORLEVEL% NEQ 0 (
    pause
    exit
)

rem Open solution file.
for /f %%f in ('dir /b "*.sln"') do (
    set solution=%%f
    goto found
)
:found

echo -- Opening solution file...
start "" "%solution%"
