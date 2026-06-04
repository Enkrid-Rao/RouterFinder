@echo off

set "PATH=C:\Program Files\JetBrains\CLion 2025.2.5\bin\cmake\win\x64\bin;%PATH%"
set "PATH=C:\Program Files\JetBrains\CLion 2025.2.5\bin\mingw\bin;%PATH%"

cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
build\RouteFinder.exe %*
