@echo off

rem Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
rem Software distributed under the permissive MIT License.
echo -- Cleaning CMake directories...

rem Remove temporary directories.
:delete_folders
for /d %%x in ("CMake-*") do rd /s /q "%%x" >nul 2>&1

rem Retry if some directories cannot be deleted.
for /d %%x in ("CMake-*") do (
    echo Could not delete some files. Making another attempt in 4 seconds...
    timeout /t 4 >nul 2>&1
    goto delete_folders
)

rem Remove temporary files.
for %%x in ("*.tmp") do rm /s /q "%%x" >nul 2>&1
