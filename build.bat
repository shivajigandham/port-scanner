@echo off
echo ========================================
echo   Advanced C++ Port Scanner Builder
echo ========================================

:: Check for g++ (MinGW)
where g++ >nul 2>nul
if %ERRORLEVEL% equ 0 (
    echo [OK] Found g++ (MinGW). Compiling...
    g++ port_scanner.cpp -o port_scanner.exe -lws2_32 -std=c++11
    if %ERRORLEVEL% equ 0 (
        echo [SUCCESS] port_scanner.exe created!
    ) else (
        echo [ERROR] Compilation failed.
    )
    pause
    exit /b
)

:: Check for cl (Visual Studio)
where cl >nul 2>nul
if %ERRORLEVEL% equ 0 (
    echo [OK] Found cl (Visual Studio). Compiling...
    cl /EHsc port_scanner.cpp /link ws2_32.lib /out:port_scanner.exe
    if %ERRORLEVEL% equ 0 (
        echo [SUCCESS] port_scanner.exe created!
    ) else (
        echo [ERROR] Compilation failed.
    )
    pause
    exit /b
)

echo [ERROR] No C++ compiler (g++ or cl) found in PATH.
echo Please install MinGW-w64 or Visual Studio and try again.
pause
