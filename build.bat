@echo off
chcp 65001 >nul
title Building OVR SkinChanger...
cd /d "%~dp0"

echo [*] Building cleaner.exe...
g++ -std=c++20 -O3 -s -static cleaner\cleaner.cpp -o cleaner.exe -lole32 -lshell32 -ladvapi32
if %ERRORLEVEL% NEQ 0 (
    echo [-] Failed to build cleaner.exe!
    pause
    exit /b 1
)

echo [*] Building ovr-skinchanger.dll...
if not exist build (
    cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
)
cmake --build build -j 12
if %ERRORLEVEL% NEQ 0 (
    echo [-] Failed to build ovr-skinchanger.dll!
    pause
    exit /b 1
)

strip --strip-unneeded build\ovr-skinchanger.dll
copy /y build\ovr-skinchanger.dll ovr-skinchanger.dll >nul
copy /y build\ovr-skinchanger.dll maydevilcry.dll >nul

if exist "C:\Users\project_320\Desktop\maydevilcry" (
    copy /y build\ovr-skinchanger.dll "C:\Users\project_320\Desktop\maydevilcry\maydevilcry.dll" >nul
    copy /y cleaner.exe "C:\Users\project_320\Desktop\maydevilcry\cleaner.exe" >nul
)

echo.
echo [+] BUILD SUCCESSFUL!
echo [+] cleaner.exe: D:\ovr-skinchanger\cleaner.exe
echo [+] ovr-skinchanger.dll: D:\ovr-skinchanger\ovr-skinchanger.dll
