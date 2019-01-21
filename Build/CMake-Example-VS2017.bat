@echo off

rem Copyright (c) 2018-2019 Piotr Doan. All rights reserved.
echo -- Generating CMake project...

rem Create empty project directory.
mkdir "%cd%/CMake-Example-VS2017" >nul 2>&1
cd "%cd%/CMake-Example-VS2017"

rem Generate solution files.
cmake -G "Visual Studio 15" %~dp0/../Example/

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
