@echo off
echo ========================================
echo   Advanced C++ Port Scanner Builder
echo ========================================

set MINGW_PATH=D:\mingw64\bin\g++.exe

:: Check if our new compiler exists
if exist "%MINGW_PATH%" (
    echo [OK] Found MinGW64 at D:\mingw64. Compiling...
    "%MINGW_PATH%" port_scanner.cpp -o port_scanner.exe -lws2_32 -std=c++11
    if %ERRORLEVEL% equ 0 (
        echo [SUCCESS] port_scanner.exe created!
    ) else (
        echo [ERROR] Compilation failed.
    )
    pause
    exit /b
)

:: Fallback check for g++ in PATH
where g++ >nul 2>nul
if %ERRORLEVEL% equ 0 (
    echo [OK] Found g++ in PATH. Compiling...
    g++ port_scanner.cpp -o port_scanner.exe -lws2_32 -std=c++11
    if %ERRORLEVEL% equ 0 (
        echo [SUCCESS] port_scanner.exe created!
    ) else (
        echo [ERROR] Compilation failed.
    )
    pause
    exit /b
)

echo [ERROR] No C++ compiler found.
echo Please ensure MinGW64 is installed at D:\mingw64.
pause
