@echo off
setlocal
cd /d "%~dp0..\.."

set CONFIG=Release
if /I "%~1"=="Debug" set CONFIG=Debug

set BUILD_DIR=build_sdl

echo ========================================================
echo  GuiKit SDL Build (%CONFIG%) - SDL3 + ThorVG + FreeType
echo ========================================================

:: Stop running guikit_demo.exe if open so linker can overwrite it cleanly
taskkill /F /IM guikit_demo.exe >nul 2>&1

if not exist "%BUILD_DIR%\guikit_bundle.sln" if not exist "%BUILD_DIR%\build.ninja" if not exist "%BUILD_DIR%\Makefile" (
    echo [INFO] Configuring CMake SDL3 + ThorVG + FreeType build directory ^(%BUILD_DIR%^)...
    cmake -B "%BUILD_DIR%" -S . -DGUIKIT_BUNDLE_BUILD_SDL=ON -DGUIKIT_BUNDLE_BUILD_NATIVE=OFF -DGUIKIT_BUNDLE_BUILD_DEMO=ON -DGUIKIT_BUNDLE_BUILD_TESTS=OFF -DGUIKIT_TEXT_USE_FREETYPE=ON
    if errorlevel 1 (
        echo [ERROR] CMake configuration failed!
        exit /b 1
    )
)

echo [INFO] Building guikit_demo (%CONFIG%)...
cmake --build "%BUILD_DIR%" --config %CONFIG% --target guikit_demo --parallel
if errorlevel 1 (
    echo [ERROR] SDL build failed!
    exit /b 1
)

echo.
echo ========================================================
echo  SDL build succeeded ^(SDL3 + ThorVG + FreeType^)!
if exist "%BUILD_DIR%\%CONFIG%\guikit_demo.exe" (
    echo  Binary: %BUILD_DIR%\%CONFIG%\guikit_demo.exe
) else (
    echo  Binary: %BUILD_DIR%\guikit_demo.exe
)
echo ========================================================
endlocal
