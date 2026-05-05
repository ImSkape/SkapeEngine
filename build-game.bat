:: build.bat - full regenerate + both configs
@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

echo Regenerating CMake...
cmake -B build-game -G "Visual Studio 17 2022" -A x64 -DENGINE_EDITOR=OFF
if %errorlevel% neq 0 (
    echo CMake generation failed.
    pause
    exit /b %errorlevel%
)

echo Building Debug...
cmake --build build-game --config Debug -- /m
if %errorlevel% neq 0 (
    echo Debug build failed.
    pause
    exit /b %errorlevel%
)

echo Building Release...
cmake --build build-game --config Release -- /m
if %errorlevel% neq 0 (
    echo Release build failed.
    pause
    exit /b %errorlevel%
)

echo All builds succeeded.
pause