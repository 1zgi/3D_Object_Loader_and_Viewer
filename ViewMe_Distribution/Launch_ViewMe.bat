@echo off
echo.
echo ViewMe - 3D Model Viewer v1.1
echo ===============================
echo.

REM Check if executable exists
if not exist "ViewMe.exe" (
    echo ERROR: ViewMe.exe not found!
    echo Please make sure this batch file is in the same directory as ViewMe.exe
    pause
    exit /b 1
)

echo File Association Fix: APPLIED
echo Status: Ready for .obj file associations
echo.
echo Choose how to start ViewMe:
echo.
echo 1. Start ViewMe (clean start - use Browse button or drag & drop)
echo 2. Test file association (recommended after first run)
echo 3. Exit
echo.

set /p choice="Enter your choice (1-3): "

if "%choice%"=="1" (
    echo.
    echo Starting ViewMe v1.1...
    echo Tip: Use 'Browse Models...' button or drag & drop .obj files
    echo.
    "ViewMe.exe"
) else if "%choice%"=="2" (
    echo.
    echo To test file association:
    echo 1. Find any .obj file on your computer
    echo 2. Right-click the .obj file
    echo 3. Select "Open with" ^> "Choose another app"
    echo 4. Browse and select this ViewMe.exe
    echo 5. Check "Always use this app to open .obj files"
    echo 6. Click OK
    echo.
    echo ViewMe will now open .obj files when double-clicked!
    echo.
    pause
    echo Starting ViewMe for testing...
    "ViewMe.exe"
) else if "%choice%"=="3" (
    echo Exiting...
    exit /b 0
) else (
    echo Invalid choice. Starting ViewMe...
    "ViewMe.exe"
)

echo.
echo ViewMe closed.
pause
