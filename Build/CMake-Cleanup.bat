@echo off

rem Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
rem Software distributed under the permissive MIT License.

echo -- Cleaning CMake directories...
:delete_folders
for /d %%x in ("CMake-*") do rd /s /q "%%x" >nul 2>&1

for /d %%x in ("CMake-*") do (
    echo Could not delete some files. Making another attempt in 4 seconds...
    timeout /t 4 >nul 2>&1
    goto delete_folders
)

echo -- Cleaning temporary files...
for %%x in ("*.tmp") do rm /s /q "%%x" >nul 2>&1
