@echo off
chcp 65001 >nul
title Building OVR Cleaner...
cd /d "%~dp0"

echo [*] Compiling cleaner.exe with g++...
g++ -std=c++20 -O3 -s -static cleaner\cleaner.cpp -o cleaner.exe -lole32 -lshell32 -ladvapi32

if %ERRORLEVEL% EQU 0 (
    echo [+] Successfully built cleaner.exe!
) else (
    echo [-] Failed to build cleaner.exe!
)
