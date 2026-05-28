cmake_minimum_required(VERSION 3.16)
project(Graphfinder)

set(CMAKE_CXX_STANDARD 20)

add_executable(Graphfinder
    main.cpp
    src/data.cpp
    src/commandParse.cpp
    src/Dijkstra.cpp
)

target_include_directories(Graphfinder PRIVATE include . src)
