@echo off
setlocal
cd /d "%~dp0..\.."

set "ROOT_DIR=%CD%"
set "CONFIG=%~1"
if "%CONFIG%"=="" set "CONFIG=Release"

set "BUILD_DIR=%ROOT_DIR%\build_sdl"
set "EXE_PATH=%BUILD_DIR%\%CONFIG%\guikit_demo.exe"
if not exist "%EXE_PATH%" if exist "%BUILD_DIR%\guikit_demo.exe" set "EXE_PATH=%BUILD_DIR%\guikit_demo.exe"

if not exist "%EXE_PATH%" (
    echo [guikit] guikit_demo.exe not found. Building %CONFIG% ^(SDL3 + ThorVG + FreeType^) first...
    call "%~dp0sdl-build.cmd" "%CONFIG%"
    if errorlevel 1 exit /b 1
    set "EXE_PATH=%BUILD_DIR%\%CONFIG%\guikit_demo.exe"
    if not exist "%EXE_PATH%" if exist "%BUILD_DIR%\guikit_demo.exe" set "EXE_PATH=%BUILD_DIR%\guikit_demo.exe"
)

if not exist "%EXE_PATH%" (
    echo [ERROR] Build failed, guikit_demo.exe not found.
    exit /b 1
)

echo [guikit] Launching SDL Desktop Demo ^(SDL3 + ThorVG + FreeType^): "%EXE_PATH%"
start "" /D "%ROOT_DIR%" "%EXE_PATH%"
endlocal
