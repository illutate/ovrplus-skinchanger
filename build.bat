@echo off
chcp 65001 >nul
title Building MayDevilCry v1.04...
cd /d "%~dp0"

echo [*] Building cleaner.exe...
g++ -std=c++20 -O3 -s -static cleaner\cleaner.cpp -o cleaner.exe -lole32 -lshell32 -ladvapi32 -luuid
if %ERRORLEVEL% NEQ 0 (
    echo [-] Failed to build cleaner.exe!
    pause
    exit /b 1
)

echo [*] Building maydevilcry.dll...
if not exist build (
    cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
)
cmake --build build -j 12
if %ERRORLEVEL% NEQ 0 (
    echo [-] Failed to build maydevilcry.dll!
    pause
    exit /b 1
)

strip --strip-unneeded build\maydevilcry.dll
copy /y build\maydevilcry.dll maydevilcry.dll >nul

echo.
echo [+] BUILD SUCCESSFUL (v1.04)!
echo [+] maydevilcry.dll: %~dp0maydevilcry.dll
echo [+] cleaner.exe:    %~dp0cleaner.exe
