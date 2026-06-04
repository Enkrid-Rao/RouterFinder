@echo off

:: Auto-find cmake
for /f "delims=" %%i in ('where cmake 2^>nul') do set "CMAKE=%%~dpi"
if defined CMAKE goto :found_cmake
for /d %%d in ("%ProgramFiles%\JetBrains\CLion*") do (
    if exist "%%d\bin\cmake\win\x64\bin\cmake.exe" (
        set "CMAKE=%%d\bin\cmake\win\x64\bin"
        goto :found_cmake
    )
)
for /d %%d in ("%ProgramFiles%\Microsoft Visual Studio\2022\*") do (
    if exist "%%d\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" (
        set "CMAKE=%%d\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin"
        goto :found_cmake
    )
)
:found_cmake
if defined CMAKE set "PATH=%CMAKE%;%PATH%"

:: Auto-find g++
for /f "delims=" %%i in ('where g++ 2^>nul') do set "MINGW=%%~dpi"
if defined MINGW goto :found_mingw
for /d %%d in ("%ProgramFiles%\JetBrains\CLion*") do (
    if exist "%%d\bin\mingw\bin\g++.exe" (
        set "MINGW=%%d\bin\mingw\bin"
        goto :found_mingw
    )
)
:found_mingw
if defined MINGW set "PATH=%MINGW%;%PATH%"

where cmake >nul 2>&1 || (echo Error: cmake not found & pause & exit /b 1)
where g++   >nul 2>&1 || (echo Error: g++ not found   & pause & exit /b 1)

cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
echo Build complete!
