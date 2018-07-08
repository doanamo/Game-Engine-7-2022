@echo off

rem
rem Copyright (c) 2018 Piotr Doan. All rights reserved.
rem

rem Remove temporary directories.
:delete_folders
for /d %%x in ("CMake-*") do rd /s /q "%%x"

rem Retry if some directories cannot be deleted.
for /d %%x in ("CMake-*") do (
    echo Could not delete some files. Making another attempt in 4 seconds...
    timeout /t 4 >nul
    goto delete_folders
)

rem Remove temporary files.
for %%x in ("*.tmp") do rm /s /q "%%x"
