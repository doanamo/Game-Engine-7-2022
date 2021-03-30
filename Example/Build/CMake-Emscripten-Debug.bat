@echo off
setlocal

call "../../Build/CMake-Emscripten-Release.bat" "../." "CMake-Emscripten-Debug" "Debug"
