@echo off
title Aura Flow Launcher
echo 🚀 Starting Aura Flow...

:: Check if python is in PATH
python --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ❌ Python not found! Please install Python 3.11+ and add it to PATH.
    pause
    exit /b
)

:: Run the app
:: We use pythonw.exe to hide the console window if you want it silent, 
:: but for now we'll use python.exe so you can see any errors.
echo 🎙️ Aura Flow is running in the background. Check the system tray!
start "" pythonw main.py

exit
