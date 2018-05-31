@echo off

rem Remove existing directory.
rd /s /q "CMake-VS2017" >nul 2>&1

rem Create an empty directory.
mkdir "CMake-VS2017"
cd "CMake-VS2017"

rem Generate solution files.
cmake -G "Visual Studio 15" ./..

rem Prevent console from closing.
pause
