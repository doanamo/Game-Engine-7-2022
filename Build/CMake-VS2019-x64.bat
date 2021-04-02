@echo off

rem Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
rem Software distributed under the permissive MIT License.
echo -- Generating CMake project...

rem Collect command line arguments.
set inputDir=%1
set outputDir=%2
set arch=%3

if [%inputDir%] == [] set inputDir="."
if [%outputDir%] == [] set outputDir="CMake-VS2019-x64"
if [%arch%] == [] set arch=x64

rem Create empty project directory.
mkdir %outputDir% >nul 2>&1
cd %outputDir%

rem Generate solution files.
set archArg=-A %arch%

cmake -G "Visual Studio 16" %archArg% "../%inputDir%"

if %ERRORLEVEL% NEQ 0 (
    pause
    exit
)

rem Open main solution file.
for /f %%f in ('dir /b "*.sln"') do (
    set solution=%%f
    goto found
)
:found

echo -- Opening solution file...
start "" "%solution%"
