@echo off
chcp 65001 >nul
title Aura Flow C++ — Deploy & Run

set EXE_DIR=d:\PROJECTS\AURA WISP\aura-flow-cpp\out\build\x64-Release
set EXE=%EXE_DIR%\AuraFlow.exe
set WINDEPLOYQT=C:\Qt\6.5.3\msvc2019_64\bin\windeployqt.exe

echo ========================================
echo   Aura Flow C++ — Deploy Script
echo ========================================

:: Check if EXE exists
if not exist "%EXE%" (
    echo.
    echo ERROR: AuraFlow.exe not found!
    echo Expected: %EXE%
    echo.
    echo Please build the project in Visual Studio first.
    pause
    exit /b 1
)

:: Deploy Qt DLLs
echo.
echo [1/2] Deploying Qt DLLs...
"%WINDEPLOYQT%" "%EXE%" --no-translations
if %errorlevel% neq 0 (
    echo ERROR: windeployqt failed!
    pause
    exit /b 1
)
echo      Done!

:: Launch
echo.
echo [2/2] Launching AuraFlow...
echo.
cd /d "%EXE_DIR%"
start "" "%EXE%"

echo Aura Flow is running in the system tray!
timeout /t 3 >nul
