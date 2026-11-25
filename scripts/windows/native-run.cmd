@echo off
setlocal EnableDelayedExpansion
cd /d "%~dp0..\.."

set "ROOT_DIR=%CD%"
set "CONFIG=%~1"
if "%CONFIG%"=="" set "CONFIG=Release"

set "BUILD_DIR=%ROOT_DIR%\build_native"
set "EXE_PATH=%BUILD_DIR%\%CONFIG%\guikit_demo_native.exe"
if not exist "%EXE_PATH%" if exist "%BUILD_DIR%\guikit_demo_native.exe" set "EXE_PATH=%BUILD_DIR%\guikit_demo_native.exe"

if not exist "%EXE_PATH%" (
    echo [guikit] guikit_demo_native.exe not found. Building %CONFIG% ^(native-only, no SDL3/ThorVG^) first...
    call "%~dp0native-build.cmd" "%CONFIG%"
    if errorlevel 1 exit /b 1
    set "EXE_PATH=%BUILD_DIR%\%CONFIG%\guikit_demo_native.exe"
    if not exist "!EXE_PATH!" if exist "%BUILD_DIR%\guikit_demo_native.exe" set "EXE_PATH=%BUILD_DIR%\guikit_demo_native.exe"
)

if not exist "%EXE_PATH%" (
    echo [ERROR] Build failed, guikit_demo_native.exe not found.
    exit /b 1
)

echo [guikit] Launching Native Desktop Demo ^(zero SDL3 / zero ThorVG^): "%EXE_PATH%"
"%EXE_PATH%"
set "EXIT_CODE=%ERRORLEVEL%"
endlocal & exit /b %EXIT_CODE%
