@echo off
setlocal
cd /d "%~dp0..\.."

set CONFIG=Release
if /I "%~1"=="Debug" set CONFIG=Debug

set BUILD_DIR=build_native

echo ========================================================
echo  GuiKit Native Build (%CONFIG%) - Zero SDL3 / Zero ThorVG
echo ========================================================

:: Stop running guikit_demo_native.exe if open so linker can overwrite it cleanly
taskkill /F /IM guikit_demo_native.exe >nul 2>&1

if not exist "%BUILD_DIR%\guikit_bundle.sln" if not exist "%BUILD_DIR%\build.ninja" if not exist "%BUILD_DIR%\Makefile" (
    echo [INFO] Configuring CMake native-only build directory ^(%BUILD_DIR%^)...
    cmake -B "%BUILD_DIR%" -S . -DGUIKIT_BUNDLE_BUILD_SDL=OFF -DGUIKIT_BUNDLE_BUILD_NATIVE=ON -DGUIKIT_BUNDLE_BUILD_DEMO=ON -DGUIKIT_BUNDLE_BUILD_TESTS=OFF
    if errorlevel 1 (
        echo [ERROR] CMake configuration failed!
        exit /b 1
    )
)

echo [INFO] Building guikit_demo_native (%CONFIG%)...
cmake --build "%BUILD_DIR%" --config %CONFIG% --target guikit_demo_native --parallel
if errorlevel 1 (
    echo [ERROR] Native build failed!
    exit /b 1
)

echo.
echo ========================================================
echo  Native build succeeded ^(zero SDL3, zero ThorVG^)!
if exist "%BUILD_DIR%\%CONFIG%\guikit_demo_native.exe" (
    echo  Binary: %BUILD_DIR%\%CONFIG%\guikit_demo_native.exe
) else (
    echo  Binary: %BUILD_DIR%\guikit_demo_native.exe
)
echo ========================================================
endlocal
