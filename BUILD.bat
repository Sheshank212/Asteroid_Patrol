@echo off
cd /d "%~dp0"

echo ========================================
echo ASTEROID PATROL - Build Script
echo ========================================
echo.

REM Find Visual Studio
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
    set "VS_PATH=%%i"
)

echo Setting up compiler environment...
call "%VS_PATH%\VC\Auxiliary\Build\vcvars32.bat" >nul

if not exist bin mkdir bin

echo.
echo Compiling...
echo.

REM Compile all source files
cl /nologo /EHsc /std:c++17 /MD ^
   /DGLEW_STATIC /D_CRT_SECURE_NO_WARNINGS /DGLM_FORCE_RADIANS ^
   /I"include" ^
   /I"C:\Users\shesh\source\repos\Libraries\include" ^
   /Fe"bin\AsteroidPatrol.exe" ^
   main.cpp src\*.cpp ^
   /link ^
   "C:\Users\shesh\source\repos\Libraries\lib\glew32s.lib" ^
   "C:\Users\shesh\source\repos\Libraries\lib\glfw3.lib" ^
   opengl32.lib gdi32.lib user32.lib shell32.lib ^
   legacy_stdio_definitions.lib ucrt.lib vcruntime.lib

if %ERRORLEVEL% EQU 0 (
    REM Copy shader files to bin directory
    if not exist bin\shaders mkdir bin\shaders
    xcopy /Y /Q shaders\*.glsl bin\shaders\ >nul

    echo.
    echo ========================================
    echo BUILD SUCCESS!
    echo ========================================
    echo.
    echo Executable: bin\AsteroidPatrol.exe
    echo.
    echo Press any key to run the game...
    pause >nul
    cd bin
    AsteroidPatrol.exe
) else (
    echo.
    echo ========================================
    echo BUILD FAILED!
    echo ========================================
    pause
)
